/*
 * simple compiler developed by arttnba3, Xidian University
 * 2021.11
*/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "message/message.h"
#include "scanner/scanner.h"
#include "parser/parser.h"
#include "semantic/semantic.h"

using namespace std;

int main(int argc, char **argv, char **envp)
{
    char    choice;

    // check the params
    if (argc < 2)
        err_exit("Usage: ./A3compiler [object file]", "", -EFAULT);

    // init the scanner, open the input file
    if (!init_scanner(argv[1]))
        err_exit("failed to open the source file: %s", argv[1],  -EFAULT);

    
    // test for scanner
    if (argv[2] && !strcmp(argv[2], "test"))
    {
        cout << "Testing Token..." << endl;
        struct token *t;
        t = get_token();
        for (int i = 0; !(t->type == ERRORTOKEN || t->type == NONTOKEN); i++)
        {
            cout << "token " << i << ": " << "letme: " << ((t->lexme) ? t->lexme : "NULL") << " type: " << t->type << " val: " << t->val << " func ptr: " << (size_t)(t->func_ptr) << endl;
            t = get_token();
        }
        exit(0);
    }

    // parsing the code, translate into python code
    cout << "Start to parse..." << endl;
    parser();

    // close the scanner
    cout << "Done." << endl;
    close_scanner();
    close_drawer();

    // run the python to draw the pic
    while (true)
    {
        cout << "Interpretation done. run it now? [Y/n]" << endl;
        cin.get(choice);
        if (toupper(choice) == 'Y' || choice == '\n')
        {
            cout << "Press enter to exit..." << endl;
            system("python3 ./drawer.py");
            break;
        }
        else if (toupper(choice) == 'N')
        {
            cout << "Result saved in ./drawer.py" << endl;
            exit(0);
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }

}