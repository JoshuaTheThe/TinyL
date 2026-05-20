
#include <std/math.h>
#include <math.h>

void Builtin_Absolute(size_t argc)
{
        if (argc < 1) return;
        VALUE Number = RT_RequireType(TYPE_INT);
        if (Number.as.integer < 0) Number.as.integer = -Number.as.integer;
        RT_Push(Number);
}

void Builtin_IntegerSqrt(size_t argc)
{
        if (argc < 1) return;
        VALUE Number = RT_RequireType(TYPE_INT);
        Number.as.integer = sqrt(Number.as.integer);
        RT_Push(Number);
}

void Builtin_Random(size_t argc)
{
        RT_Push(INT(rand()));
}

void Builtin_SeedRandom(size_t argc)
{
        if (argc < 1) return;
        VALUE Seed = RT_RequireType(TYPE_INT);
        srand(Seed.as.integer); // returns none
}
