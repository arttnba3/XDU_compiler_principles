#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include "tools.h"

using std::cout, std::endl, std::strcmp, std::exit;
using namespace a3lib;

void a3lib::err_report(const string &s)
{
    cout << "A3DrawLang: error: " << s << endl;
}

void a3lib::err_exit(const string &s)
{
    cout << "A3DrawLang: fatal error: " << s << endl;
    cout << "compilation terminated.\n";
    exit(EXIT_FAILURE);
}

void a3lib::help_info(void)
{
    cout << "Usage: A3DrawLang [options] file\n";
    cout << "Options:\n";
    cout << "  --help                   Display this information.\n";
    cout << "  -o <file>                Place the output into <file>.\n";
    cout << "                           If it's not set, it'll be 'drawer.py'";
    cout << ".\n";
    cout << "  --test                   Scan and print the tokens only.\n";

    cout << endl;
    cout << "For bug reporting, please issue it on:\n";
    cout << "https://github.com/arttnba3/XDU_compiler_principles/issues\n";
}

class cmd_args* a3lib::args_parser(int argc, const char **argv) 
{
    class cmd_args *res = new class cmd_args;
    enum {
        NEW_ARG,
        ARG_GET_VAL,
    } status = NEW_ARG;
    /* maybe we'll have more available options in the future... */
    enum {
        OUTPUT_FILE,
    } arg_type;
    int i;

    for (i = 0; i < argc; i++) {
        switch (status) {
        case NEW_ARG:
            if (!strcmp(argv[i], "--h") || !strcmp(argv[i], "--help")) {
                res->set_run_type(HELP_INFO);
                goto help_out;
            } else if (!strcmp(argv[i], "-o")) {
                arg_type = OUTPUT_FILE;
            } else if (!strcmp(argv[i], "--test")) {
                res->set_run_type(TEST_SCANNER);
                continue;
            } else if (res->get_input_file() == "") {
                res->set_input_file(argv[i]);
                continue;
            } else {
                throw cmd_args_error(CMD_UNKNOW_ARG, argv[i]);
            }
            status = ARG_GET_VAL;
            break;
        case ARG_GET_VAL:
            switch (arg_type) {
            case OUTPUT_FILE:
                res->set_output_file(argv[i]);
                break;
            default:
                err_report("inner parsing error(invalid arg_type)");
                break;
            }
            status = NEW_ARG;
            break;
        default:
            err_report("inner parsing error(invalid status)");
            break;
        } 
    }

    if (status == ARG_GET_VAL) {
        throw cmd_args_error(CMD_NO_ARG_VAL, argv[i - 1]);
    } else if (res->get_input_file() == "") {
        throw cmd_args_error(CMD_NO_INPUT, "");
    }

help_out:
    return res;
}

