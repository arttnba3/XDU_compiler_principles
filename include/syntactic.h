#ifndef _A3COMPILER_SYNTACTIC_H_
#define _A3COMPILER_SYNTACTIC_H_

#include <queue>
#include "scanner.h"
#include "symbol.h"

namespace a3compiler {
    using std::string, std::queue;

    /* node of abstract syntactic tree */
    struct ExprNode {
        enum token_type opcode;

        /* to save memory, we use a union there */
        union {
            /* the node is an operator */
            struct {
                struct ExprNode *left;
                struct ExprNode *right;
            } case_operator;

            /* the node is a call of function */
            struct {
                struct ExprNode *arg;
                double (*fn) (double);
            } case_func;

            /* the node is a const value (right value) */
            double case_const;
        };

        /* the node is a variable (left value) or an assignment */
        struct {
            string lexeme;
            struct ExprNode *val;
        } case_param;
    };
    typedef struct ExprNode ExprNode;

    /* syntactic parser */
    class SyntacticParser {
    private:
        queue<Token*> token_queue;
        size_t line;
        Scanner *scanner;
        Token* fetch_token(void);
        void match_token(Token *token, enum token_type type);
        Token* fetch_and_match_token(enum token_type type);
        void token_back(Token *token);
        ExprNode* make_node(enum token_type opcode, void *arg1, void *arg2);
        ExprNode* atom(void);
        ExprNode* component(void);
        ExprNode* factor(void);
        ExprNode* term(void);
        ExprNode* expression(void);
        ExprNode* origin_statement(void);
        ExprNode* rot_statement(void);
        ExprNode* scale_statement(void);
        ExprNode* for_draw_statement(void);
        ExprNode* var_statement(Token *token);
    public:
        SyntacticParser(Scanner *scanner);
        ExprNode* parse_statement(void);
    };
};

#endif