
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "message/message.h"
#include "scanner.h"

using namespace std;

FILE *input_file;
size_t line_no;

char token_buffer[MAX_TOKEN_LEN];
size_t token_buffer_loc;

// use micro to avoid cast of frequent calls of functions
#define is_upper(chr) \
    (chr >= 'A' && chr <= 'Z')

#define is_lower(chr) \
    (chr >= 'a' && chr <= 'z')

#define is_alpha(chr) \
    (is_upper(chr) || is_lower(chr))

#define is_digit(chr) \
    (chr >= '0' && chr <= '9') 

#define is_alnum(chr) \
    (is_alpha(chr) || is_digit(chr))

#define to_upper(chr) \
    (is_lower(chr) ? (chr ^ ' ') : chr)

#define is_space(chr) \
    (chr == ' ' || chr == '\t' || chr == '\r' || chr == '\n' || chr == '\v' || chr == '\f')

#define add_to_token_buffer(chr) \
do \
{ \
    token_buffer[token_buffer_loc++] = chr; \
}while(0)

#define back_char(chr) \
do \
{ \
    ungetc(chr, input_file); \
}while(0)

#define add_to_token_table(per_token) \
do \
{ \
    if (append_token_table_size >= MAX_TOKEN_TABLE_NUM) \
        err_exit("token table limited.", nullptr, -ENOMEM); \
    append_token_table[append_token_table_size++] = per_token;\
}while(0)

static inline char get_char(void)
{
    char chr = getc(input_file);
    return to_upper(chr);
}

static inline char* create_letme_buf(void)
{
    char    *buf;

    buf = new char[token_buffer_loc];
    if (!buf)
        err_exit("run out of memory.", nullptr, -ENOMEM);
    memset(buf, 0, sizeof(char) * token_buffer_loc);
    memcpy(buf, token_buffer, token_buffer_loc);
    return buf;
}

bool init_scanner(char *file_name)
{
    input_file = fopen(file_name, "r");
    line_no = 1;
    if (input_file == nullptr)
        return false;
    return true;
}

void close_scanner(void)
{
    fclose(input_file);
}

struct token* token_table_query(char *letme)
{
    struct token    *t;

    // find from the builtin token table
    for (int i = 0; i < BUILTIN_TOKEN_TABLE_NUM; i++)
    {
        if (!strcmp(bulit_in_token_table[i].lexme, letme))
            return &bulit_in_token_table[i];
    }

    // find from the append token table
    for (int i = 0; i < append_token_table_size; i++)
    {
        if (!strcmp(append_token_table[i]->lexme, letme))
            return append_token_table[i];
    }

    // not found, an errortoken for it
    t = new struct token;
    if (!t)
        err_exit("run out of memory.", nullptr, -ENOMEM);
    memset(t, 0, sizeof(struct token));
    t->type = ERRORTOKEN;

    return t;
}

struct token* get_token(void)
{
    struct token    *per_token;
    char            per_chr;

    // init token and buffer
    per_token = new struct token;
    if (!per_token)
        err_exit("run out of memory.", nullptr, -ENOMEM);
    memset(per_token, 0, sizeof(per_token));
    memset(token_buffer, 0, sizeof(char) * MAX_TOKEN_LEN);
    token_buffer_loc = 0;

    // clear the space
    while (true)
    {
        per_chr = get_char();
        if (per_chr == '\n')
            line_no++;
        if (per_chr == EOF)
        {
            per_token->type = NONTOKEN;
            return per_token;
        }
        if (!is_space(per_chr)) // token start
            break;
    }
    add_to_token_buffer(per_chr);   //  first non-space char, add to token buffer as first char

    // check for alpha
    if (is_alpha(per_chr) || per_chr == '_')
    {
        while (true)
        {
            per_chr = get_char();
            if (!is_alnum(per_chr))
                break;
            add_to_token_buffer(per_chr);
        }
        back_char(per_chr); // not the char of this token, push it back

        // query if the token has been record
        delete per_token;
        per_token = token_table_query(token_buffer);
        // new token, add to token table
        if (per_token->type == ERRORTOKEN)
        {
            per_token->type = T;
            per_token->lexme = create_letme_buf();
            add_to_token_table(per_token);
        }
    }
    // check for numeric
    else if (is_digit(per_chr))
    {
        while (true)
        {
            per_chr = get_char();
            if (!is_digit(per_chr))
                break;
            add_to_token_buffer(per_chr);
        }
        if (per_chr == '.')
        {
            add_to_token_buffer(per_chr);
            while (true)
            {
                per_chr = get_char();
                if (!is_digit(per_chr))
                    break;
                add_to_token_buffer(per_chr);
            }
        }
        back_char(per_chr);     // push non-digit char back
        per_token->lexme = create_letme_buf();
        per_token->type = CONST_ID;
        per_token->val = atof(per_token->lexme);
    }
    // other condition
    else
    {
        switch (per_chr)
        {
            case ',':
                    per_token->type = COMMA;
                    break;
            case ';':
                    per_token->type = SEMICO;
                    break;
            case '(':
                    per_token->type = L_BRACKET;
                    break;
            case ')':
                    per_token->type = R_BRACKET;
                    break;
            case '+':
                    per_token->type = PLUS;
                    break;
            case '*':
                    per_chr = get_char();
                    if (per_chr == '*') // ** for power
                    {
                        per_token->type = POWER;
                    }
                    else                // * for mul
                    {
                        per_token->type = MUL;
                        back_char(per_chr);
                    }
                    break;
            case '-':
                    per_chr = get_char();
                    if (per_chr == '-') // -- for comment in a line
                    {
                        while (per_chr != '\n' && per_chr != EOF)
                            per_chr = get_char();
                        back_char(per_chr); // first char of next line or EOF, push back
                        per_token = get_token();
                    }
                    else                // * for minus
                    {
                        per_token->type = MINUS;
                        back_char(per_chr);
                    }
                    break;
            case '/':
                    per_chr = get_char();
                    if (per_chr == '/') // // for power
                    {
                        while (per_chr != '\n' && per_chr != EOF)
                            per_chr = get_char();
                        back_char(per_chr); // first char of next line or EOF, push back
                        per_token = get_token();
                    }
                    else                // / for div
                    {
                        per_token->type = MUL;
                        back_char(per_chr);
                    }
                    break;
            default:                // invalid char appear
                    per_token->type = ERRORTOKEN;
                    break;
        }
    }

    return per_token;
}