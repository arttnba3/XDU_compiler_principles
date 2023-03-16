#include <cstring>
#include "symbol.h"

using std::memcpy;

using namespace a3compiler;

bool SymbolTable::add_token(Token *t)
{
    /* table is already full */
    if (this->sym_nr >= MAX_TOKEN_NUM) {
        return false;
    }

    this->table[this->sym_nr++] = t;
    return true;
}

Token* SymbolTable::find_token(string &lexeme)
{
    for (int i = 0; i < this->sym_nr; i++) {
        if (this->table[i]->lexeme == lexeme) {
            return this->table[i];
        }
    }

    return nullptr;
}

void SymbolTable::init_table(Token *builtin_table, int nr)
{
    this->sym_nr = 0;

    for (int i = 0; i < nr; i++) {
        Token *t = new Token;
        
        t->type = builtin_table[i].type;
        t->lexeme = builtin_table[i].lexeme;
        t->val = builtin_table[i].val;
        t->fn = builtin_table[i].fn;

        if (!this->add_token(t)) {
            delete t;
            break;
        }
    }
}

SymbolTable::SymbolTable(void)
{
    this->init_table(bulit_in_symbol_table, BUILTIN_TOKEN_NR);
}

SymbolTable::SymbolTable(Token *builtin_table, int nr)
{
    this->init_table(builtin_table, nr);
}