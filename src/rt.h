
#ifndef RT_H
#define RT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tok.h>

typedef enum
{
        TYPE_NONE,
        TYPE_FN,
        TYPE_INT,
        TYPE_BUILTIN,
} TYPE;

typedef struct VALUE
{
        TYPE Type;
        union
        {
                int64_t integer;
                struct
                {
                        char  *start;
                        TOKEN  arguments[16];
                        size_t argc;
                } function;
                
                void (*builtin)(size_t argc);
        } as;
} VALUE;

typedef struct VARIABLE
{
        TOKEN Token;
        char Name[16];
        VALUE Value;
        struct VARIABLE *Next;
} VARIABLE;

typedef struct SCOPE
{
        VARIABLE *Variables;
        bool Private; // cannot access parent;
} SCOPE;

VALUE RT_Pop(void);
void RT_Push(VALUE Value);
VARIABLE *RT_CreateVariable(TOKEN tok, VALUE Value);
VARIABLE *RT_FindVariable(TOKEN tok);
void RT_CleanupVariables(void);
void RT_EnterScope(bool Private);
void RT_ExitScope(void);
void RT_DebugPrint(VALUE *Value, char *Name);
VALUE RT_RequireType(TYPE Type);
void RT_Cleanup(void);
void RT_EmptyStack(void);
void RT_Initialise(void);

#endif
