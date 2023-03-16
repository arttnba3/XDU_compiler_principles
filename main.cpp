#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "tools.h"
#include "a3compiler.h"

using namespace std;

a3lib::cmd_args* parse_arg(int argc, const char **argv)
{
    a3lib::cmd_args *cmd_args;

    try {
        cmd_args = a3lib::args_parser(argc - 1, argv + 1);
    } catch (a3lib::cmd_args_error &err) {
        a3lib::err_exit(err.what());
    }

    return cmd_args;
}

int main(int argc, const char **argv)
{
    a3compiler::A3Compiler *compiler;
    a3lib::cmd_args *cmd_args;
    ifstream *input;
    ofstream *output;

    /* init for cmd args */
    cmd_args = parse_arg(argc, argv);

    /* for --help to print help info only */
    if (cmd_args->is_help()) {
        a3lib::help_info();
        return EXIT_SUCCESS;
    }

    input = new ifstream(cmd_args->get_input_file(), ios::in | ios::binary);
    output = new ofstream(cmd_args->get_output_file(), ios::out | ios::trunc);

    if (!input) {
        a3lib::err_exit("failed to open input file: '" \
                        + cmd_args->get_input_file() + "'");
    }

    if (!output) {
        a3lib::err_exit("failed to open output file: '" \
                        + cmd_args->get_output_file() + "'");
    }

    compiler = new a3compiler::A3Compiler(input, output, nullptr);

    try {
        /* for --test option to print all valid tokens only */
        if (cmd_args->get_run_type() == a3lib::TEST_SCANNER) {
            compiler->test_token();
            return 0;
        } else { /* normal compiling */
            compiler->compile();
        }
    } catch (a3compiler::A3CompilerError &e) {
        a3lib::err_exit(e.what());
    }

    input->close();
    output->close();

    /* run the python to draw the pic */
    while (true) {
        char choice;

        cout << "Interpretation done. run it now? [Y/n]" << endl;

        cin.get(choice);

        if (toupper(choice) == 'Y' || choice == '\n') {
            cout << "Press enter to exit..." << endl;
            system(("python3 " + cmd_args->get_output_file()).c_str());
            break;
        } else if (toupper(choice) == 'N') {
            cout << "Result saved in " + cmd_args->get_output_file() << endl;
            exit(0);
        } else {
            cout << "Invalid choice" << endl;
        }
    }

    return 0;
}