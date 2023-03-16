#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

#include "tools.h"
#include "symbol.h"
#include "scanner.h"

using std::ios, std::memset, std::string, std::atof, std::printf;

using namespace a3compiler;

Scanner::Scanner(ifstream *input, SymbolTable *table)
{
    this->input = input;
    this->table = table;
}

char Scanner::next_char(void)
{
    char ch;

    if (!this->input || this->input->eof()) {
        return EOF;
    }

    ch = this->input->get();

    /* case-insensitive for us */
    ch = to_upper(ch);

    return ch;
}

void Scanner::token_println(Token *t)
{
    if (t == nullptr) {
        return ;
    }

    printf("lexeme: %s, type: %d, val: %lf, fn: %p\n",
           t->lexeme.c_str(), t->type, t->val, t->fn);
}

void Scanner::char_back(void)
{
    this->input->seekg(-1, ios::cur);
}

Token* Scanner::get_token(void)
{
    Token *token;
    string lexeme = "";
    char ch;

    /* clear the space char */
    while (true) {
        ch = this->next_char();

        if (!is_space(ch)) {
            break;
        }
    }

    /* end of file */
    if (ch == EOF) {
        token = new Token;
        token->type = NONTOKEN;
        return token;
    }

    /* this's the first non-space char, add it to buf */
    lexeme += ch;

    /* check for alpha, which means that it's a variable or a key word */
    if (is_alpha(ch) || ch == '_') {
        while (true) {
            ch = this->next_char();

            if (!(is_alnum(ch) || ch == '_')) {
                if (ch != EOF) {
                    this->char_back();
                }
                break;
            }

            lexeme += ch;
        }

        token = this->table->find_token(lexeme);

        /* seems that it's a new token, add it to table */
        if (!token) {
            token = new Token;
            token->type = T;
            token->lexeme = lexeme;

            if (!this->table->add_token(token)) {
                delete token;
                token = nullptr;
            }
        }
    } else if (is_digit(ch)) { /* it may be a const */
        while (true) {
            ch = this->next_char();

            if (!is_digit(ch)) {
                if (ch != EOF) {
                    this->char_back();
                }
                break;
            }

            lexeme += ch;
        }

        /* fractional part */
        if (ch == '.') {
            lexeme += ch;
            this->next_char();
            while (true) {
                ch = this->next_char();

                if (!is_digit(ch)) {
                    if (ch != EOF) {
                        this->char_back();
                    }
                    break;
                }

                lexeme += ch;
            }
        }

        token = new Token;
        token->type = CONST_ID;
        token->lexeme = lexeme;
        token->val = atof(lexeme.c_str());
    } else {
        token = new Token;

        switch (ch) {
        /* separators */
        case ';':
            token->type = SEMICO;
            break;
        case '(':
            token->type = L_BRACKET;
            break;
        case ')':
            token->type = R_BRACKET;
            break;
        case ',':
            token->type = COMMA;
            break;
        /* operator */
        case '+':
            token->type = PLUS;
            break;
        case '-':
            ch = this->next_char();

            if (ch == '-') {
                /* it's a comment, ignore other chars until next line */
                do {
                    ch = this->next_char();
                } while (ch != EOF && ch != '\n');

                /* first char of next line, push back */
                if (ch != EOF) {
                    this->char_back();
                }

                /* we'd like to ignore the comment, so just read next token */
                delete token;
                token = this->get_token();
            } else { /* it's a sub operation */
                token->type = MINUS;

                /* first char of next token, push back */
                if (ch != EOF) {
                    this->char_back();
                }
            }
            break;
        case '*':
            ch = this->next_char();

            if (ch == '*') { /* it's a power operation */
                token->type = POWER;
            } else {    /* it's a multiplication operation */
                token->type = MUL;

                /* first char of next token, push back */
                if (ch != EOF) {
                    this->char_back();
                }
            }
            break;
        case '/':
            ch = this->next_char();

            if (ch == '/') {
                /* it's a comment, ignore other chars until next line */
                do {
                    ch = this->next_char();
                } while (ch != EOF && ch != '\n');

                /* first char of next line, push back */
                if (ch != EOF) {
                    this->char_back();
                }

                /* we'd like to ignore the comment, so just read next token */
                delete token;
                token = this->get_token();
            } else { /* it's a division operation */
                token->type = DIV;

                /* first char of next token, push back */
                if (ch != EOF) {
                    this->char_back();
                }
            }
            break;
        default:
            token->type = ERRORTOKEN;
            break;
        }
    }

    return token;
}