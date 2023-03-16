#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "message/message.h"
#include "scanner/scanner.h"
#include "semantic/semantic.h"
#include "parser.h"

using namespace std;

static struct expr_node* make_expr_node(enum token_type opcode, void *arg1, void *arg2);
static struct expr_node* atom(void);
static struct expr_node* component(void);
static struct expr_node* factor(void);
static struct expr_node* term(void);
static struct expr_node* expression(void);

/*
 * get a new token with checking its type
*/
void fetch_token()
{
    cur_token = get_token();
    if (cur_token->type == ERRORTOKEN)
        err_exit("error at fetching token.", nullptr, -EFAULT);
}

/* 
 * check if the current token suit the type we need.
 * if it does, read the next token automatically.
*/
void match_token(enum token_type type)
{
    if (cur_token->type != type)
        err_exit("unexpeced token: %s", (cur_token->lexme ? cur_token->lexme : "(null)"), -EFAULT);
    fetch_token();
}

/*
 * delete a abstract syntax tree
 * for binary operation, the left and right node should be delete in recursion in addition
 * for function call, the child should be delete in addition
 * after all above, delete the node directly
*/
static void del_tree(struct expr_node *tree)
{
    if (tree == nullptr)
        return ;
    switch (tree->opcode)
    {
        // binary operator
        case PLUS:
        case MINUS:
        case MUL:
        case DIV:
        case POWER:
                del_tree(tree->content.case_operator.left);
                del_tree(tree->content.case_operator.right);
                break;
        // function call
        case FUNC:
                del_tree(tree->content.case_func.child);
                break;
        default:
                break;
    }

    delete tree;
}

/*
 * create a expr_node instance
 * @param opcode type of the operator
 * @param arg1 for operator node, it's left node; for func node, it's the func ptr, for const_id or var, it's the value
 * @param arg2 for operator node, it's right node; for func node, it's child node; useless for const_id and var
*/
static struct expr_node* make_expr_node(enum token_type opcode, void *arg1, void *arg2)
{
    struct expr_node    *new_node;

    new_node = new struct expr_node;
    if (!new_node)
        err_exit("run out of memory.", nullptr, -ENOMEM);
    new_node->opcode = opcode;
    switch (opcode)
    {
        case CONST_ID:
                new_node->content.case_const = *reinterpret_cast<double*>(arg1);
                break;
        case T:
                new_node->content.case_param = reinterpret_cast<double*>(arg1);
                break;
        case FUNC:
                new_node->content.case_func.func_ptr = reinterpret_cast<double (*)(double)>(arg1);
                new_node->content.case_func.child = reinterpret_cast<struct expr_node*>(arg2);
                break;
        default:
                new_node->content.case_operator.left = reinterpret_cast<struct expr_node*>(arg1);
                new_node->content.case_operator.right = reinterpret_cast<struct expr_node*>(arg2);
                break; 
    }

    return new_node;
}

/*
 * get an atom expression including const val, variable, func and bracket
 * for func, the basic form to get should be "func(expression)"
 * for bracket, the form should be "(expression)"
 * regex: atom -> CONST_ID | T | FUNC L_BRACKET Expression R_BRACKET | L_BRACKET Expression R_BRACKET
*/
static struct expr_node* atom(void)
{
    struct token        *per_token;
    struct expr_node    *new_node;
    struct expr_node    *per_expression;

    per_token = cur_token;
    switch (per_token->type)
    {
        case CONST_ID:
                match_token(CONST_ID);
                new_node = make_expr_node(CONST_ID, &per_token->val, nullptr);
                break;
        case T:
                match_token(T);
                new_node = make_expr_node(T, &(per_token->val), nullptr);
                break;
        case FUNC:
                match_token(FUNC);
                match_token(L_BRACKET);
                per_expression = expression();
                new_node = make_expr_node(FUNC, reinterpret_cast<void*>(per_token->func_ptr), per_expression);
                match_token(R_BRACKET);
                break;
        case L_BRACKET:
                match_token(L_BRACKET);
                new_node = expression();
                match_token(R_BRACKET);
                break;
        default:
                err_exit("Invalid expression: at line: %d", (char*)line_no, -EFAULT);
                break;
    }

    return new_node;
}

/*
 * to get power expression
 * regex: component -> atom[power component]
*/
static struct expr_node* component(void)
{
    struct expr_node    *left;
    struct expr_node    *right;
    left = atom();
    if (cur_token->type == POWER)
    {
        match_token(POWER);
        right = component();
        left = make_expr_node(POWER, left, right);
    }
    return left;
}

/*
 * to get factor expression
 * regex: PLUS Factor | MINUS Factor | Component
*/
static struct expr_node* factor(void)
{
    struct expr_node    *left;
    struct expr_node    *right;

    switch(cur_token->type)
    {
        case PLUS:
                match_token(PLUS);
                right = factor();
                left = new expr_node;
                if (!left)
                    err_exit("run out of memory.", nullptr, -ENOMEM);
                left->opcode = PLUS;
                left->content.case_const = 0.0;
                right = make_expr_node(PLUS, left, right);
                break;
        case MINUS:
                match_token(MINUS);
                right = factor();
                left = new expr_node;
                if (!left)
                    err_exit("run out of memory.", nullptr, -ENOMEM);
                left->opcode = CONST_ID;
                left->content.case_const = 0.0;
                right = make_expr_node(MINUS, left, right);
                break;
        default:
                right = component();
                break;
    }

    return right;
}

/*
 * construct the AST of binary operation "MUL" and "DIV" in recursion
 * regex: TERM -> Factor { (MUL | DIV) Factor }
*/
static struct expr_node* term(void)
{
    struct expr_node    *left;
    struct expr_node    *right;
    token_type          type;

    left = factor();
    while (cur_token->type == MUL || cur_token->type == DIV)
    {
        type = cur_token->type;
        match_token(type);
        right = factor();
        left = make_expr_node(type, 
                reinterpret_cast<void*>(left), 
                reinterpret_cast<void*>(right)
        );
    }
    return left;
}

/*
 * construct the AST of binary operation "PLUS" and "MINUS" in recursion
 * regex: TERM { (PLUS | MINUS) TERM } 
*/
static struct expr_node* expression(void)
{
    struct expr_node    *left;
    struct expr_node    *right;
    token_type          type;

    left = term();
    while (cur_token->type == PLUS || cur_token->type == MINUS)
    {
        type = cur_token->type;
        match_token(type);
        right = term();
        left = make_expr_node(type, 
                reinterpret_cast<void*>(left), 
                reinterpret_cast<void*>(right)
        );
    }
    return left;
}

/*
 * parse an origin statement
 * regex: ORIGIN IS L_BRACKET EXPRESSION COMMA EXPRESSION R_BRACKET 
 * work of matching SEMICO will be done by parser() automatically 
*/
void origin_statement(void)
{
    struct expr_node    *node;

    // match the format
    match_token(ORIGIN);
    match_token(IS);
    match_token(L_BRACKET);

    // get the value
    node = expression();
    origin_x = get_expr_val(node);
    del_tree(node);

    match_token(COMMA);

    node = expression();
    origin_y = get_expr_val(node);
    del_tree(node);

    // match final R_BRACKET
    match_token(R_BRACKET);
}

/*
 * parse a scale statement
 * regex: SCALE IS L_BRACKET EXPRESSION COMMA EXPRESSION R_BRACKET 
 * work of matching SEMICO will be done by parser() automatically 
*/
void scale_statement(void)
{
    struct expr_node    *node;

    // match the format
    match_token(SCALE);
    match_token(IS);
    match_token(L_BRACKET);

    // get the value
    node = expression();
    scale_x = get_expr_val(node);
    del_tree(node);

    match_token(COMMA);

    node = expression();
    scale_y = get_expr_val(node);
    del_tree(node);

    // match final R_BRACKET
    match_token(R_BRACKET);
}

/*
 * parse a rot statement
 * regex: ROT IS EXPRESSION
 * work of matching SEMICO will be done by parser() automatically 
*/
void rot_statement(void)
{
    struct expr_node    *node;

    // match the format
    match_token(ROT);
    match_token(IS);

    // get the value
    node = expression();
    rot = get_expr_val(node);
    del_tree(node);
}

/*
 * parse a for statement
 * regex: FOR T FROM EXPRESSION TO EXPRESSION SETP EXPRESSION DRAW L_BRACKET EXPRESSION COMMA EXPRESSION R_BRACKET
 * work of matching SEMICO will be done by parser() automatically 
*/
void for_statement(void)
{
    struct expr_node    *node;
    struct token        *t;
    double              start;
    double              end;
    double              step;
    struct expr_node    *node_x;
    struct expr_node    *node_y;

    // match the regex
    match_token(FOR);
    t = cur_token;
    match_token(T);

    match_token(FROM);
    node = expression();
    start = get_expr_val(node);
    del_tree(node);

    match_token(TO);
    node = expression();
    end = get_expr_val(node);
    del_tree(node);

    match_token(STEP);
    node = expression();
    step = get_expr_val(node);
    del_tree(node);

    match_token(DRAW);
    match_token(L_BRACKET);
    node_x = expression();
    match_token(COMMA);
    node_y = expression();
    match_token(R_BRACKET);

    // start to draw
    draw_loop(t, start, end, step, node_x, node_y);
    del_tree(node_x);
    del_tree(node_y);
}

/*
 * parse a variable statement, if not exists it'll be create automatically
 * regex: T IS EXPRESSION
 * work of matching SEMICO will be done by parser() automatically 
*/
void var_statement(void)
{
    struct expr_node    *node;
    struct token        *t;

    t = cur_token;
    match_token(T);
    match_token(IS);
    node = expression();
    t->val = get_expr_val(node);
    del_tree(node);
}

/*
 * kernel parser
*/
void parser(void)
{
    fetch_token();
    while (cur_token->type != NONTOKEN)  // analyse by line
    {
        switch(cur_token->type)
        {
            case ORIGIN:    // set origin
                    origin_statement();
                    break;
            case SCALE:     // set scarle
                    scale_statement();
                    break;
            case ROT:       // set rot
                    rot_statement();
                    break;
            case FOR:       // for statement
                    for_statement();
                    break;
            case T:         // declare a new variable
                    var_statement();
                    break;
            case SEMICO:    // a 'blank line'
                    break;
            default:
                    err_exit("", "", -EFAULT);
                    break;
        }
        match_token(SEMICO);    // for end of a line, a ';' is needed
    }
    
}