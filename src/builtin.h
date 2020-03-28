#ifndef BUILTIN_H
#define BUILTIN_H
#include "common.h"
bool is_builtin(char * name);
void exec_builtin(char *args[]);
#endif
