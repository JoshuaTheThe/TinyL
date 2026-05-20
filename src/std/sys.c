
#include <std/sys.h>
#include <time.h>
#include <unistd.h>

void Builtin_Exit(size_t argc)
{
        SETUP(argc);
        int code = 0;
        if (argc > 0)
        {
                INTARGUMENT(Value);
                code = Value.as.integer;
        }

        RT_Cleanup();
        exit(code);
}

void Builtin_Typeof(size_t argc)
{
        SETUP(argc);
        if (argc < 1) return;
        ARGUMENT(Value);
        RT_Push(INT(Value.Type)); // lazy
}

void Builtin_Sleep(size_t argc)
{
        SETUP(argc);
        if (argc < 1) return;
        INTARGUMENT(Value);
        sleep(Value.as.integer);
}

void Builtin_Time(size_t argc)
{
        SETUP(argc);
        (void)argc;
        RT_Push(INT(time(NULL)));
}
