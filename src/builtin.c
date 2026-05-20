
#include <builtin.h>
#include <rt.h>
#include <error.h>

#include <std/io.h>
#include <std/array.h>
#include <std/convert.h>
#include <std/math.h>
#include <std/sys.h>

void Builtin_AddBuiltinFunctions(void)
{
        // STD.IO
        ADD_BUILTIN("print", Builtin_Print);
        ADD_BUILTIN("input", Builtin_Input);

        // STD.ARRAY
        ADD_BUILTIN("find", Builtin_Find);
        ADD_BUILTIN("append", Builtin_Append);
        ADD_BUILTIN("remove", Builtin_Remove);
        ADD_BUILTIN("len", Builtin_Len);
        ADD_BUILTIN("capacity", Builtin_Cap);
        ADD_BUILTIN("asstr", Builtin_Str);
        ADD_BUILTIN("asarr", Builtin_Arr);

        // STD.CONVERT
        ADD_BUILTIN("toint", Builtin_ToInt);
        ADD_BUILTIN("tostr", Builtin_ToStr);

        // STD.MATH
        ADD_BUILTIN("abs", Builtin_Absolute);
        ADD_BUILTIN("sqrt", Builtin_IntegerSqrt);
        ADD_BUILTIN("random", Builtin_Random);
        ADD_BUILTIN("randomseed", Builtin_SeedRandom);

        // STD.SYS
        ADD_BUILTIN("exit", Builtin_Exit);
        ADD_BUILTIN("typeof", Builtin_Typeof);
        ADD_BUILTIN("time", Builtin_Time);
}
