#ifndef _A3_COMPILER_COMPILE_ERR_H_
#define _A3_COMPILER_COMPILE_ERR_H_

#include <iostream>

namespace a3compiler {
    using std::string, std::exception, std::to_string;
    class A3CompilerError : exception {
    private:
        size_t line;
        string error_msg;
    public:
        A3CompilerError(const string &msg, size_t line)
        {
            this->error_msg = msg;
            this->line = line;
        }
        const char* what(void) const throw()
        {
            return (this->error_msg+"at line: "+to_string(this->line)).c_str();
        }
    };
};

#endif