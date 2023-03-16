#ifndef _A3COMPILER_SYMBOL_H_
#define _A3COMPILER_SYMBOL_H_

#define MAX_TOKEN_LEN 1024

#include <cmath>
#include <iostream>

using std::string;

namespace a3compiler {
    enum token_type {
        /* key words */
        ORIGIN = 0, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM,
        /* variables */
        T,
        /* separator ; ( ) , */
        SEMICO, L_BRACKET, R_BRACKET, COMMA,
        /* operator + - * / ** */
        PLUS, MINUS, MUL, DIV, POWER,
        /* function */
        FUNC,
        /* const value */
        CONST_ID,
        /* end of token stream */
        NONTOKEN,
        /* unrecognized token */
        ERRORTOKEN
    };

    #define MAX_LEXEME_LEN 1024

    struct Token {
        enum token_type type;
        string lexeme;  /* only need for function & variable */
        double val;     /* only need for const value */
        double (*fn) (double);  /* only need for function */
    };

    typedef struct Token Token;

    /* built-in symbol table */
    static Token bulit_in_symbol_table[] = {
        { CONST_ID,     "PI",       3.1415926,  nullptr     },
        { CONST_ID,     "E",        2.71828,    nullptr     },
        { T,            "T",        0.0,        nullptr     },
        { FUNC,         "SIN",      0.0,        std::sin    },
        { FUNC,         "COS",      0.0,        std::cos    },
        { FUNC,         "TAN",      0.0,        std::tan    },
        { FUNC,         "LN",       0.0,        std::log    },
        { FUNC,         "EXP",      0.0,        std::exp    },
        { FUNC,         "SQRT",     0.0,        std::sqrt   },
        { ORIGIN,       "ORIGIN",   0.0,        nullptr     },
        { SCALE,        "SCALE",    0.0,        nullptr     },
        { ROT,          "ROT",      0.0,        nullptr     },
        { IS,           "IS",       0.0,        nullptr     },
        { FOR,          "FOR",      0.0,        nullptr     },
        { FROM,         "FROM",     0.0,        nullptr     },
        { TO,           "TO",       0.0,        nullptr     },
        { STEP,         "STEP",     0.0,        nullptr     },
        { DRAW,         "DRAW",     0.0,        nullptr     },
    };
    #define BUILTIN_TOKEN_NR \
        (sizeof(bulit_in_symbol_table) / sizeof(Token))

    /* appended symbol table */
    #define MAX_TOKEN_NUM 1024

    /* symbol table */
    class SymbolTable {
    private:
        Token *table[MAX_TOKEN_LEN];
        int sym_nr;
        void init_table(Token *builtin_table, int nr);
    public:
        SymbolTable(void);
        SymbolTable(Token *builtin_table, int nr);
        bool add_token(Token *t);
        Token* find_token(string &lexeme);
    };
};

#endif