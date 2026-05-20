
#include <std/sys.h>
#include <time.h>
#include <unistd.h>

void Builtin_Exit(size_t argc)
{
        int code = 0;
        if (argc > 0)
        {
                VALUE Value = RT_RequireType(TYPE_INT);
                code = Value.as.integer;
        }

        RT_Cleanup();
        exit(code);
}

void Builtin_Typeof(size_t argc)
{
        if (argc < 1) return;
        VALUE Value = RT_Pop();
        RT_Push(INT(Value.Type)); // lazy
}

void Builtin_Sleep(size_t argc)
{
        if (argc < 1) return;
        VALUE Value = RT_RequireType(TYPE_INT);
        sleep(Value.as.integer);
}

void Builtin_Time(size_t argc)
{
        (void)argc;
        RT_Push(INT(time(NULL)));
}
