#include "a3compiler.h"
#include "compile_err.h"

using namespace a3compiler;

A3Compiler::A3Compiler(ifstream *input, ofstream *output, 
                       SymbolTable *symbol_table)
{
    this->input = input;
    this->output = output;

    if (symbol_table == nullptr) {
        this->symbol_table = new SymbolTable();
    } else {
        this->symbol_table = symbol_table;
    }

    this->scanner = new a3compiler::Scanner(input, this->symbol_table);
    this->syntactic_parser = new a3compiler::SyntacticParser(this->scanner);
    this->semantic_parser = new a3compiler::SemanticParser(this->symbol_table, 
                                                           output);
}

void A3Compiler::test_token(void)
{
    while (true) {
        a3compiler::Token *t = this->scanner->get_token();
        this->scanner->token_println(t);

        if (t->type == a3compiler::ERRORTOKEN) {
            throw A3CompilerError("INVALID TOKEN", -1);
        } else if (t->type == a3compiler::NONTOKEN) {
            break;
        }
    }
}

void A3Compiler::compile(void)
{
    a3compiler::ExprNode *stmt;

    *this->output << "from tkinter import *\n";
    *this->output << "tk = Tk()\n";
    *this->output << "tk.title(\"A3DrawLang Interpreter\")\n";
    *this->output << "canvas = Canvas(tk, width = 1200, height = 800)\n";
    *this->output << "canvas.pack()\n";

    do {
        stmt = syntactic_parser->parse_statement();
        if (!stmt) {
            break;
        }

        this->semantic_parser->parse_statement(stmt);
    } while (true);

    *this->output << "input()\n";
}