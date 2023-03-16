#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstddef>

#include "message.h"

using namespace std;

/*
 * print the error message and stop the process
 * @param msg the error message to print before exit the process
 * @param err append error info
 * @param errcode code to exit the process
*/
void err_exit(const char *msg, const char *err, int err_code)
{
    printf("a3compiler fatal error: ");
    printf(msg, err);   // warning: a format str vulnerability there, do not use this func unless you're arttnba3!
    puts("\ncompilation terminated.");
    exit(err_code);
}