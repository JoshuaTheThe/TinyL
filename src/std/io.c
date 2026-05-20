
#include <std/io.h>

void Builtin_Print(size_t argc)
{
        SETUP(argc);
        for (size_t i = 0; i < argc; ++i)
        {
                ARGUMENT(Value);
                RT_MiniDebugPrint(&Value);
        }

        RT_Push((VALUE){.Type = TYPE_INT, .as.integer = argc});
}

void Builtin_Input(size_t argc)
{
        SETUP(argc);
        if (argc > 0)
        {
                ARGUMENT(Prompt);
                if (Prompt.Type == TYPE_ARRAY && Prompt.as.array.is_string)
                {
                        for (size_t i = 0; i < Prompt.as.array.count; i++)
                                putchar(Prompt.as.array.items[i].as.integer);
                }
        }

        char buffer[1024];
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
                RT_Push((VALUE){.Type = TYPE_NONE});
                return;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
                buffer[len - 1] = '\0';

        VALUE result = {.Type = TYPE_ARRAY};
        RT_ArrayInit(&result);
        result.as.array.is_string = true;

        for (char *p = buffer; *p; p++)
                RT_Append(&result, INT(*p));
        RT_Push(result);
}

void Builtin_ReadFile(size_t argc) {} // todo
void Builtin_WriteFile(size_t argc) {} // todo
