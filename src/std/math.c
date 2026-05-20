
#include <std/math.h>
#include <math.h>

void Builtin_Absolute(size_t argc)
{
        SETUP(argc);
        if (argc < 1) return;
        INTARGUMENT(Number);
        if (Number.as.integer < 0) Number.as.integer = -Number.as.integer;
        RT_Push(Number);
}

void Builtin_IntegerSqrt(size_t argc)
{
        SETUP(argc);
        if (argc < 1) return;
        INTARGUMENT(Number);
        Number.as.integer = sqrt(Number.as.integer);
        RT_Push(Number);
}

void Builtin_Random(size_t argc)
{
        SETUP(argc);
        RT_Push(INT(rand()));
}

void Builtin_SeedRandom(size_t argc)
{
        SETUP(argc);
        if (argc < 1) return;
        INTARGUMENT(Seed);
        srand(Seed.as.integer); // returns none
}
