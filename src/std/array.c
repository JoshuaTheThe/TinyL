
#include <std/array.h>

void Builtin_Find(size_t argc)
{
        SETUP(argc);
        if (argc < 2)
        {
                RT_Push((VALUE){.Type = TYPE_NONE});
                return;
        }
        VALUE Value = RT_Pop();
        VALUE Array = RT_RequireType(TYPE_ARRAY);
        RT_Push(Array); RT_Push(Value);
        for (size_t i = 0; i < Array.as.array.count; ++i)
        {
                if (RT_Is(&Array.as.array.items[i], &Value))
                {
                        RT_Push(INT(i));
                        return;
                }
        }

        RT_Push(INT(-1));
}

void Builtin_Append(size_t argc)
{
        SETUP(argc);
        if (argc < 2)
        {
                error("append requires at least array and one value", (TOKEN){0});
                for (size_t i = 0; i < argc; i++)
                        RT_Pop();
                RT_Push((VALUE){.Type = TYPE_NONE});
                abort();
                return;
        }

        VALUE Array = RT_Pop();
        if (Array.Type != TYPE_ARRAY)
        {
                error("append last argument must be array", (TOKEN){0});
                return;
        }

        VALUE values[6];
        for (size_t i = 0; i < argc - 1; i++)
                values[argc - 2 - i] = RT_Pop();
        for (size_t i = 0; i < argc - 1; i++)
                RT_Append(&Array, values[i]);
        RT_Push(Array);
}

void Builtin_Remove(size_t argc)
{
        SETUP(argc);
        if (argc != 2)
        {
                error("remove expects array and index", (TOKEN){0});
                return;
        }

        VALUE Index = RT_Pop();
        VALUE Array = RT_Pop();

        if (Array.Type != TYPE_ARRAY)
        {
                error("remove first argument must be array", (TOKEN){0});
                return;
        }

        if (Index.Type != TYPE_INT)
        {
                error("remove second argument must be integer index", (TOKEN){0});
                return;
        }

        int64_t idx = Index.as.integer;

        if (idx < 0 || idx >= (int64_t)Array.as.array.count)
        {
                error("index %ld out of bounds (size %zu)", (TOKEN){0}, idx, Array.as.array.count);
                return;
        }

        VALUE removed = Array.as.array.items[idx];
        for (size_t i = idx; i < Array.as.array.count - 1; i++)
                Array.as.array.items[i] = Array.as.array.items[i + 1];

        Array.as.array.count--;
        RT_Push(Array);
}

void Builtin_Len(size_t argc)
{
        SETUP(argc);
        if (argc < 1)
                return;
        ARRARGUMENT(Array);
        RT_Push(INT(Array.as.array.count));
}

void Builtin_Cap(size_t argc)
{
        SETUP(argc);
        if (argc < 1)
                return;
        ARRARGUMENT(Array);
        RT_Push(INT(Array.as.array.capacity));
}

void Builtin_Str(size_t argc)
{
        SETUP(argc);
        if (argc < 1)
                return;
        ARRARGUMENT(Array);
        RT_Pop();
        Array.as.array.is_string = true;
        RT_Push(Array);
}

void Builtin_Arr(size_t argc)
{
        SETUP(argc);
        if (argc < 1)
                return;
        ARRARGUMENT(Array);
        RT_Pop();
        Array.as.array.is_string = false;
        RT_Push(Array);
}

void Builtin_Slice(size_t argc) {} // todo