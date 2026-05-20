
#ifndef STD_SYS_H
#define STD_SYS_H

#include <builtin.h>
#include <rt.h>
#include <error.h>

void Builtin_Exit(size_t argc);
void Builtin_Typeof(size_t argc);
void Builtin_Time(size_t argc);
void Builtin_Sleep(size_t argc);

#endif
