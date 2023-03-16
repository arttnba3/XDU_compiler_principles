#ifndef A3COMPILER_PARSER_H
#define A3COMPILER_PARSER_H

#include "scanner/scanner.h"

/*
 * definition of a node of the abstract syntax tree
*/
struct expr_node
{
    enum token_type opcode;
    union
    {
        struct                              // tree node
        {
            struct expr_node    *left;
            struct expr_node    *right;
        }case_operator;                 // the node is a operator
        struct 
        {
            struct expr_node    *child;
            double              (*func_ptr)(double);
        }case_func;                     // the node is a call of function
        double      case_const;         // the node is a const value    (right val)
        double      *case_param;         // the node is a param          (left val)
    }content;
};

static struct token *cur_token;

extern void parser(void);

#endif