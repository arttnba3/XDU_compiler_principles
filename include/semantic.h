#ifndef _A3COMPILER_SEMANTIC_H_
#define _A3COMPILER_SEMANTIC_H_

#include <fstream>
#include "syntactic.h"
#include "symbol.h"

namespace a3compiler {
    using std::ofstream;

    class SemanticParser {
    private:
        /* compiler part */
        size_t cur_line;
        SyntacticParser *syntactic_parser;
        SymbolTable *symbol_table;
        ofstream *output;
        /* basic param for drawing */
        double original_x, original_y;
        double rot;
        double scale_x, scale_y;

        double get_node_val(ExprNode *node);
        void draw_pixel(double x, double y);
        void origin_statement(ExprNode *node);
        void rot_statement(ExprNode *node);
        void scale_statement(ExprNode *node);
        void for_draw_statement(ExprNode *node);
        void var_statement(ExprNode *node);
    public:
        SemanticParser(SymbolTable *table, ofstream *output);
        void parse_statement(ExprNode *node);
    };
};

#endif