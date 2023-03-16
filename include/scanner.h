#ifndef _A3COMPILER_SCANNER_H_
#define _A3COMPILER_SCANNER_H_

#include <iostream>
#include <fstream>
#include "symbol.h"

namespace a3compiler {
    using std::ifstream, std::string, std::ios;

    class Scanner {
    private:
        SymbolTable *table;
        ifstream *input;
        char next_char(void);
        void char_back(void);
    public:
        Scanner(ifstream *input, SymbolTable *table);
        Token* get_token(void);
        void token_println(Token *t);
    };
};

#endif
