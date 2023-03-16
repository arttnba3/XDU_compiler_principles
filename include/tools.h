#ifndef _A3COMPILER_TOOL_H_
#define _A3COMPILER_TOOL_H_

#include <iostream>

namespace a3lib {
    using std::string, std::exception;

    enum cmd_parse_error_type {
        CMD_NO_INPUT,
        CMD_NO_ARG_VAL,
        CMD_UNKNOW_ARG,
    };

    class cmd_args_error : public exception {
    private:
        enum cmd_parse_error_type res;
        string error_msg;
    public:
        cmd_args_error(enum cmd_parse_error_type error_type, 
                       const char *error_val)
        {
            switch (error_type) {
                case CMD_NO_INPUT:
                    this->error_msg = "no input files";
                    break;
                case CMD_NO_ARG_VAL:
                    this->error_msg = "missing argument to";
                    break;
                case CMD_UNKNOW_ARG:
                    this->error_msg = "unrecognized command line option";
                    break;
                default:
                    this->error_msg = "unknown inner error";
                    break;
            }

            if (error_type != CMD_NO_INPUT) {
                this->error_msg += " \'";
                this->error_msg += error_val;
                this->error_msg += "\'";
            }
        }

        const char* what(void) const throw(){ return this->error_msg.c_str(); }
    };

    enum run_type {
        HELP_INFO,
        TEST_SCANNER,
        FULL_COMPILE,   /* default */
    };

    class cmd_args {
    private:
        string input_file, output_file;
        enum run_type run_type;
    public:
        cmd_args(void)
        {
            this->run_type = FULL_COMPILE;
            this->input_file = "";
            this->output_file = "./drawer.py";
        }
        const string& get_input_file(void) { return this->input_file; }
        void set_input_file(const string &s) { this->input_file = s; }
        const string& get_output_file(void) { return this->output_file; }
        void set_output_file(const string &s) { this->output_file = s; }
        enum run_type get_run_type(void) { return this->run_type; }
        void set_run_type(enum run_type run_type) { this->run_type = run_type; }
        bool is_help() { return this->run_type == HELP_INFO; }
    };

    void err_report(const string &s);
    void err_exit(const string &s);
    void help_info(void);
    class cmd_args *args_parser(int argc, const char **argv);
};

/* use micro to avoid the cast of frequent calls of functions */
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
    (chr==' '||chr=='\t'||chr=='\r'||chr=='\n'||chr=='\v'||chr=='\f')

#endif