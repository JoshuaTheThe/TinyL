
#include <builtin.h>
#include <rt.h>

// Builtin functions

void Builtin_Print(size_t argc)
{
        for (int64_t i = 0; i < argc; ++i)
        {
                VALUE Value = RT_Rel(-(argc-i-1));
                RT_MiniDebugPrint(&Value);
                printf("\n");
        }

        for (int64_t i = 0; i < argc; ++i)
                RT_Pop();
        RT_Push((VALUE){.Type=TYPE_INT, .as.integer = argc});
}

void Builtin_AddBuiltinFunctions(void)
{
        RT_CreateVariable((TOKEN){.Identifier="print\0\0\0\0\0\0\0\0\0\0\0",.Kind=TOKEN_IDENTIFIER,.Number=6},
                          (VALUE){.Type=TYPE_BUILTIN,.as.builtin=Builtin_Print});
}
