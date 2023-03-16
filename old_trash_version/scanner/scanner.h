#ifndef A3COMPILER_SCANNER_H
#define A3COMPILER_SCANNER_H

#define MAX_TOKEN_LEN 1024

#include <cmath>
#include <cstdio>

using namespace std;

enum token_type
{
    // reserved word
    ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW,FOR, FROM,
    // param
    T,
    // separator ; ( ) ,
    SEMICO, L_BRACKET, R_BRACKET, COMMA,
    // operator + - * / **
    PLUS, MINUS, MUL, DIV, POWER,
    // function
    FUNC,
    // const value
    CONST_ID,
    // nullptr token, which means that the source code come to an end
    NONTOKEN,
    // invalid token type
    ERRORTOKEN
};

struct token
{
    enum token_type  type;
    char            *lexme;                     // original string of the token
    double          val;                        // for if the token is a value
    double          (*func_ptr)(double);        // for function calls
};

/*
 * at there, I devided token table into two parts,
 * one is the built-in table, recording built-in token like function sin
 * the other is the append table, for example, to record the new variable
*/

// built-in token table
static struct token bulit_in_token_table[] = 
{
    { CONST_ID,     "PI",       3.1415926,  nullptr },
    { CONST_ID,     "E",        2.71828,    nullptr },
    { T,            "T",        0.0,        nullptr },
    { FUNC,         "SIN",      0.0,        sin     },
    { FUNC,         "COS",      0.0,        cos     },
    { FUNC,         "TAN",      0.0,        tan     },
    { FUNC,         "LN",       0.0,        log     },
    { FUNC,         "EXP",      0.0,        exp     },
    { FUNC,         "SQRT",     0.0,        sqrt    },
    { ORIGIN,       "ORIGIN",   0.0,        nullptr },
    { SCALE,        "SCALE",    0.0,        nullptr },
    { ROT,          "ROT",      0.0,        nullptr },
    { IS,           "IS",       0.0,        nullptr },
    { FOR,          "FOR",      0.0,        nullptr },
    { FROM,         "FROM",     0.0,        nullptr },
    { TO,           "TO",       0.0,        nullptr },
    { STEP,         "STEP",     0.0,        nullptr },
    { DRAW,         "DRAW",     0.0,        nullptr },
};

#define BUILTIN_TOKEN_TABLE_NUM (sizeof(bulit_in_token_table) / sizeof(struct token))

#define MAX_TOKEN_TABLE_NUM 1024

static int append_token_table_size = 0;

static struct token *append_token_table[MAX_TOKEN_TABLE_NUM];

// exposed API
extern size_t line_no;
extern struct token* get_token(void);
extern bool init_scanner(char *filename);
extern void close_scanner(void);

#endif
/* A3COMPILER_SCANNER_H */