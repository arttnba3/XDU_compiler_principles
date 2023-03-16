#include <cmath>
#include <fstream>
#include <iomanip>
#include "tools.h"
#include "symbol.h"
#include "semantic.h"
#include "compile_err.h"

using namespace a3compiler;
using a3lib::err_report;
using std::pow, std::ofstream, std::setprecision, std::sin, std::cos, std::fabs;
using std::to_string;

SemanticParser::SemanticParser(SymbolTable *table, ofstream *output)
{
    this->symbol_table = table;
    this->output = output;
    this->original_x = this->original_y = 0;
}

double SemanticParser::get_node_val(ExprNode *node)
{
    double res;
    Token *token;

    if (node == nullptr) {
        throw A3CompilerError("internal semantic parser error", this->cur_line);
    }

    switch (node->opcode) {
    case PLUS:
        res = this->get_node_val(node->case_operator.left) \
            + this->get_node_val(node->case_operator.right);
        break;
    case MINUS:
        res = this->get_node_val(node->case_operator.left) \
            - this->get_node_val(node->case_operator.right);
        break;
    case MUL:
        res = this->get_node_val(node->case_operator.left) \
            * this->get_node_val(node->case_operator.right);
        break;
    case DIV:
        res = this->get_node_val(node->case_operator.left) \
            / this->get_node_val(node->case_operator.right);
        break;
    case POWER:
        res = pow(this->get_node_val(node->case_operator.left), 
                  this->get_node_val(node->case_operator.right));
        break;
    case FUNC:
        res = node->case_func.fn(this->get_node_val(node->case_func.arg));
        break;
    case CONST_ID:
        res = node->case_const;
        break;
    case T:
        token = this->symbol_table->find_token(node->case_param.lexeme);
        if (token == nullptr) {
            throw A3CompilerError("variable '" + node->case_param.lexeme + "'"\
                                    + " was not declared!", 
                                  this->cur_line);
        }
        res = token->val;
        break;
    default:
        throw A3CompilerError("invalid type of node to get val",this->cur_line);
        break;
    }

    return res;
}

void SemanticParser::draw_pixel(double x, double y)
{
    double tmp;

    /* spatial transformation first */
    x *= this->scale_x;
    y *= this->scale_y;
    tmp = x * cos(this->rot) + y * sin(this->rot);
    y = y * cos(this->rot) - x * sin(this->rot);
    x = tmp;
    x += this->original_x;
    y += this->original_y;

    /* some bad value may occur, don't draw that */
    if (fabs(x) != NAN && fabs(x) != INFINITY 
        && fabs(y) != NAN && fabs(y) != INFINITY) {
        *this->output << setprecision(10) << "canvas.create_oval(";
        *this->output << x << ", " << y << ", " << x+1 << ", " << y+1 << ")\n";
    }
}

void SemanticParser::origin_statement(ExprNode *node)
{
    this->original_x = this->get_node_val(node->case_operator.left);
    this->original_y = this->get_node_val(node->case_operator.right);
}

void SemanticParser::rot_statement(ExprNode *node)
{
    this->rot = this->get_node_val(node->case_operator.left);
}

void SemanticParser::scale_statement(ExprNode *node)
{
    this->scale_x = this->get_node_val(node->case_operator.left);
    this->scale_y = this->get_node_val(node->case_operator.right);
}

void SemanticParser::for_draw_statement(ExprNode *node)
{
    Token *var;
    ExprNode *step_val, *from, *to, *step_len, *draw_x, *draw_y;

    /**
     * We use the node->left to store a STEP node, 
     *     it's left(node FROM)'s left&right is FROM Expression and var to use
     *     it's right(node TO)'s left&right is TO & STEP Expression.
     * We use the node->right to store a DRAW node, 
     *     it's left&right is DRAW's Expression.
     * 
     */
    from = node->case_operator.left->case_operator.left->case_operator.left;
    step_val=node->case_operator.left->case_operator.left->case_operator.right;
    to = node->case_operator.left->case_operator.right->case_operator.left;
    step_len=node->case_operator.left->case_operator.right->case_operator.right;
    draw_x = node->case_operator.right->case_operator.left;
    draw_y = node->case_operator.right->case_operator.right;

    var = this->symbol_table->find_token(step_val->case_param.lexeme);
    if (var == nullptr) {
        throw A3CompilerError("variable '" + node->case_param.lexeme + "'"\
                              + " was not declared!", 
                              this->cur_line);
    }

    var->val = this->get_node_val(from);

    while (var->val < this->get_node_val(to)) {
        this->draw_pixel(this->get_node_val(draw_x),this->get_node_val(draw_y));
        var->val += this->get_node_val(step_len);
    }
}

void SemanticParser::var_statement(ExprNode *node)
{
    Token *var;

    var = this->symbol_table->find_token(node->case_param.lexeme);
    /* new variable, add to symbol table */
    if (var == nullptr) {
        var = new Token;
        var->type = T;
        var->lexeme = node->case_param.lexeme;
        var->val = 0;
        var->fn = nullptr;

        this->symbol_table->add_token(var);
    }

    var->val = this->get_node_val(node->case_param.val);
}

void SemanticParser::parse_statement(ExprNode *node)
{
    if (node == nullptr) {
        throw A3CompilerError("internal semantic parser error", this->cur_line);
    }

    switch (node->opcode) {
    case ORIGIN:
        this->origin_statement(node);
        break;
    case SCALE:
        this->scale_statement(node);
        break;
    case ROT:
        this->rot_statement(node);
        break;
    case T:
        this->var_statement(node);
        break;
    case FOR:
        this->for_draw_statement(node);
        break;
    default:
        err_report("invalid statement opcode: " + to_string(node->opcode));
        throw A3CompilerError("invalid statement opcode", this->cur_line);
        break;
    }

    this->cur_line++;
}
