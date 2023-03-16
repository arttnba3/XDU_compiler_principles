#include "tools.h"
#include "syntactic.h"
#include "compile_err.h"

using namespace a3compiler;
using a3lib::err_report;
using std::string;

SyntacticParser::SyntacticParser(Scanner *scanner)
{
    this->line = 1;
    this->scanner = scanner;
}

Token* SyntacticParser::fetch_token(void)
{
    Token *token;
    if (this->token_queue.empty()) {
        token = this->scanner->get_token();
    } else {
        token = this->token_queue.front();
        this->token_queue.pop();
    }

    return token;
}

void SyntacticParser::match_token(Token *token, enum token_type type)
{
    if (!token->type == type) {
        err_report("token mismatch:");
        this->scanner->token_println(token);
        throw A3CompilerError("token mismatch", this->line);
    }
}

Token* SyntacticParser::fetch_and_match_token(enum token_type type)
{
    Token *token = this->fetch_token();
    match_token(token, type);
    return token;
}

void SyntacticParser::token_back(Token *token)
{
    this->token_queue.push(token);
}

/**
 * @brief create an AST node
 * 
 * @param opcode opcode for the node
 * @param arg1 ptr to const val for CONST_ID, ptr to lexeme for T, 
 *             ptr to param node for FUNC, ptr to left node for operation.
 * @param arg2 ptr to function for FUNC, ptr to right node for operation.
 * @return ExprNode* new AST node
 */
ExprNode* SyntacticParser::make_node(enum token_type opcode,void*arg1,void*arg2)
{
    ExprNode *node = new ExprNode;

    node->opcode = opcode;

    switch (opcode) {
    case CONST_ID:
        node->case_const = *reinterpret_cast<double*>(arg1);
        break;
    /**
     * for T node, if the right is NULL, it represents a var only,
     * otherwise it means to assign a new value to the variable
     */
    case T:
        node->case_param.lexeme = *reinterpret_cast<string*>(arg1);
        node->case_param.val = reinterpret_cast<ExprNode*>(arg2);
        break;
    case FUNC:
        node->case_func.arg  = reinterpret_cast<ExprNode*>(arg1);
        node->case_func.fn = reinterpret_cast<double(*)(double)>(arg2);
        break;
    default:
        node->case_operator.left = reinterpret_cast<ExprNode*>(arg1);
        node->case_operator.right = reinterpret_cast<ExprNode*>(arg2);
        break;
    }

    return node;
}

/**
 * @brief Atom is a foundational component in AST, which represents
 * a basic expression of const_id / variable / function call / expression...
 * 
 * regex:   Atom-> CONST_ID
 *               | T
 *               | FUNC L_BRACKET Expression R_BRACKET
 *               | L_BRACKET Expression R_BRACKET
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::atom(void)
{
    ExprNode *node;
    Token *token;

    token = this->fetch_token();

    switch (token->type) {
    case CONST_ID:
        node = this->make_node(CONST_ID, &token->val, nullptr);
        break;
    case T:
        node = this->make_node(T, &token->lexeme, nullptr);
        break;
    case FUNC:
        this->fetch_and_match_token(L_BRACKET);
        node = this->make_node(FUNC, 
                               this->expression(), 
                               reinterpret_cast<void*>(token->fn));
        this->fetch_and_match_token(R_BRACKET);
        break;
    case L_BRACKET:
        node = this->expression();
        this->fetch_and_match_token(R_BRACKET);
        break;
    default:
        err_report("invalid token start in Atom statement");
        throw A3CompilerError("token mismatch", this->line);
        break;
    }

    return node;
}

/**
 * @brief A Component represents an Atom possibly with appended POWER operation;
 * 
 * regex: Component -> Atom [POWER Component]
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::component(void)
{
    ExprNode *left, *right;
    Token *token;

    left = this->atom();
    token = this->fetch_token();

    if (token->type == POWER) {
        right = this->component();
        left = this->make_node(POWER, left, right);
    } else {
        this->token_back(token);
    }

    return left;
}

/**
 * @brief Factor represents a simple signed Component.
 * 
 * regex: Factor -> PLUS Factor | MINUS Factor | Component
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::factor(void)
{
    ExprNode *left, *right;
    Token *token;
    static double zero = 0;

    token = this->fetch_token();
    switch (token->type) {
    case PLUS: /* we turn (+factor) into (factor) simply */
        right = this->factor();
        break;
    case MINUS: /* we turn (-factor) into (0 - factor) */
        left = this->make_node(CONST_ID, &zero, nullptr);
        right = this->make_node(token->type, left, this->factor());
        break;
    default:
        this->token_back(token);
        right = this->component();
        break;
    }

    return right;
}

/**
 * @brief Term represents a MUL/DIV operation between Factors, or a Factor only.
 * 
 * regex: Term -> Factor { ( MUL | DIV ) Factor }
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::term(void)
{
    ExprNode *left, *right;
    Token *token;

    left = this->factor();
    token = this->fetch_token();

    while (token->type == MUL || token->type == DIV) {
        right = this->factor();
        left = this->make_node(token->type, left, right);
        token = this->fetch_token();
    }

    this->token_back(token);

    return left;
}

/**
 * @brief Expression represents a expression consists of Terms with +-operation.
 * 
 * regex: Expression -> Term  { ( PLUS | MINUS) Term } 
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::expression(void)
{
    ExprNode *left, *right;
    Token *token;

    left = this->term();
    token = this->fetch_token();

    while (token->type == MUL || token->type == DIV) {
        right = this->term();
        left = this->make_node(token->type, left, right);
        token = this->fetch_token();
    }

    this->token_back(token);

    return left;
}

/**
 * @brief create an AST node that presents a ORIGIN statement, (x, y) stores in
 *      node's left & right node.
 * 
 * regex: ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::origin_statement(void)
{
    ExprNode *left, *right;

    this->fetch_and_match_token(IS);
    this->fetch_and_match_token(L_BRACKET);
    left = this->expression();
    this->fetch_and_match_token(COMMA);
    right = this->expression();
    this->fetch_and_match_token(R_BRACKET);

    return this->make_node(ORIGIN, left, right);
}

/**
 * @brief create an AST node that represents setting the ROT value of 
 *          current environment
 * 
 * regex: ORIGIN IS Expression
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::rot_statement(void)
{
    this->fetch_and_match_token(IS);
    return this->make_node(ROT, this->expression(), nullptr);
}

/**
 * @brief create an AST node that presents a SCALE statement, (x, y) stores in
 *      node's left & right node.
 * 
 * regex: SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::scale_statement(void)
{
    ExprNode *left, *right;

    this->fetch_and_match_token(IS);
    this->fetch_and_match_token(L_BRACKET);
    left = this->expression();
    this->fetch_and_match_token(COMMA);
    right = this->expression();
    this->fetch_and_match_token(R_BRACKET);

    return this->make_node(SCALE, left, right);
}

/**
 * @brief create an AST node that presents a FOR DRAW statement.
 *      We use the node->left to store a STEP node, 
 *          it's left(node FROM)'s left&right is FROM Expression and var to use
 *          it's right(node TO)'s left&right is TO & STEP Expression.
 *      We use the node->right to store a DRAW node, 
 *          it's left&right is DRAW's Expression.
 * 
 * regex: FOR Expression FROM Expression TO Expression STEP Expression
 *         DRAW L_BRACKET Expression COMMA Expression R_BRACKET
 * 
 * @return ExprNode* an AST node
 */
ExprNode* SyntacticParser::for_draw_statement(void)
{
    ExprNode *step_val, *from, *to, *step_len, *draw_x, *draw_y;
    ExprNode *node;
    Token *val_for_step;

    /* we need to make sure that it's not an expression but a variable only */
    val_for_step = this->fetch_and_match_token(T);
    step_val = this->make_node(T, &val_for_step->lexeme, nullptr);

    this->fetch_and_match_token(FROM);
    from = this->expression();

    this->fetch_and_match_token(TO);
    to = this->expression();

    this->fetch_and_match_token(STEP);
    step_len = this->expression();

    this->fetch_and_match_token(DRAW);

    this->fetch_and_match_token(L_BRACKET);
    draw_x = this->expression();

    this->fetch_and_match_token(COMMA);

    draw_y = this->expression();
    this->fetch_and_match_token(R_BRACKET);

    node = this->make_node(FOR, 
                          this->make_node(STEP, 
                                          this->make_node(FROM, from, step_val),
                                          this->make_node(TO, to, step_len)),
                          this->make_node(DRAW, draw_x, draw_y));

    return node;
}

/**
 * @brief assign a new val to a variable. If not existed, it'll be added to
 *        the symbol table.
 * 
 * regex: name_of_var IS Expression
 * 
 * @return ExprNode* an AST node.
 */
ExprNode* SyntacticParser::var_statement(Token *token)
{
    this->fetch_and_match_token(IS);
    return this->make_node(T, &token->lexeme, this->expression());
}

ExprNode* SyntacticParser::parse_statement(void)
{
    ExprNode *node = nullptr;
    Token *token;

refetch:
    /* first token of first statement */
    token = this->fetch_token();

    switch(token->type) {
    case ORIGIN:
        node = this->origin_statement();
        break;
    case SCALE:
        node = this->scale_statement();
        break;
    case ROT:
        node = this->rot_statement();
        break;
    case T:
        node = this->var_statement(token);
        break;
    case FOR:
        node = this->for_draw_statement();
        break;
    case SEMICO:    /* a blank line, just fetch the next line */
        goto refetch;
        break;
    case NONTOKEN:
        goto out;
        break;
    default:
        err_report("invalid statement start with token:");
        this->scanner->token_println(token);
        throw A3CompilerError("invalid statement start", this->line);
        break;
    }

    /* a statement should end with a SEMICO */
    this->fetch_and_match_token(SEMICO);
    this->line++;

out:
    return node;
}