
#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tok.h>

#define error(msg, ...) errorimpl(msg, __func__, __FILE__, __LINE__, ##__VA_ARGS__)

void errorimpl(char *msg, const char *func, const char *file, int line, ...);

#endif
