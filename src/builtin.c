
#include <builtin.h>
#include <rt.h>

// Builtin functions

void Builtin_Print(size_t argc)
{
        for (size_t i = 0; i < argc; ++i)
        {
                VALUE Value = RT_Rel(-(argc-i-1));
                RT_MiniDebugPrint(&Value);
        }

        for (size_t i = 0; i < argc; ++i)
                RT_Pop();
        RT_Push((VALUE){.Type=TYPE_INT, .as.integer = argc});
        printf("\n");
}

void Builtin_Find(size_t argc)
{
        if (argc < 2) {RT_Push((VALUE){.Type=TYPE_NONE}); return;}
        VALUE Value = RT_Pop();
        VALUE Array = RT_RequireType(TYPE_ARRAY);
        for (size_t i = 0; i < Array.as.array.count; ++i)
        {
                if (Array.as.array.items[i].Type != Value.Type)
                        continue;
                switch (Value.Type)
                {
                        case TYPE_NONE: RT_Push(INT(i)); return;
                        case TYPE_INT:
                                if (Array.as.array.items[i].as.integer == Value.as.integer)
                                {
                                        RT_Push(INT(i));
                                        return;
                                }
                                break;
                        case TYPE_FN:
                                if (!memcmp(&Array.as.array.items[i].as.function, &Value.as.function, sizeof(Value.as.function)))
                                {
                                        RT_Push(INT(i));
                                        return;
                                }
                                break;
                        case TYPE_BUILTIN:
                                if (Array.as.array.items[i].as.builtin == Value.as.builtin)
                                {
                                        RT_Push(INT(i));
                                        return;
                                }
                                break;
                        case TYPE_ARRAY:
                                if (Array.as.array.items[i].as.builtin == Value.as.builtin)
                                {
                                        RT_Push(INT(i));
                                        return;
                                }
                                break;
                }
        }
}

void Builtin_AddBuiltinFunctions(void)
{
        ADD_BUILTIN("print", Builtin_Print);
        ADD_BUILTIN("find", Builtin_Find);
}
