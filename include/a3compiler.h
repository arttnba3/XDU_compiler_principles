#ifndef _A3_COMPILER_H_
#define _A3_COMPILER_H_

#include <fstream>
#include "symbol.h"
#include "scanner.h"
#include "syntactic.h"
#include "semantic.h"
#include "compile_err.h"

namespace a3compiler {
    using std::ifstream, std::ofstream;
    class A3Compiler {
    private:
        ifstream *input;
        ofstream *output;
        SymbolTable *symbol_table;
        Scanner *scanner;
        SyntacticParser *syntactic_parser;
        SemanticParser *semantic_parser;
    public:
        A3Compiler(ifstream *input, ofstream *output, SymbolTable*symbol_table);
        void test_token(void);
        void compile(void);
    };
};

#endif