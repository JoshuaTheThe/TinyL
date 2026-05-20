
#ifndef RT_H
#define RT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tok.h>

#define INT(X) ((VALUE){.Type=TYPE_INT,.as.integer = X})
#define MAX_STACK_DEPTH (8)
#define MAX_SCOPE_DEPTH (1024)

typedef enum
{
        TYPE_NONE,
        TYPE_FN,
        TYPE_INT,
        TYPE_ARRAY,
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
                        TOKEN  arguments[6];
                        size_t argc;
                } function;
                
                struct
                {
                        struct VALUE *items;
                        size_t        count;
                        size_t        capacity;
                        bool          is_string;
                } array;
                
                void (*builtin)(size_t argc);
        } as;
} VALUE;

typedef struct VARIABLE
{
        TOKEN Token;
        char Name[IDENTIFIER_SIZE];
        VALUE Value;
        struct VARIABLE *Next;
} VARIABLE;

typedef struct SCOPE
{
        VARIABLE *Variables;
        VALUE *Stack;
        bool Private; // cannot access parent;
        int StackPointer;
} SCOPE;

#define RT_RequireType(type) RT_RequireTypeIMPL((type), __func__, __FILE__, __LINE__)

VALUE RT_Pop(void);
void RT_Push(VALUE Value);
VARIABLE *RT_CreateVariable(TOKEN tok, VALUE Value);
VARIABLE *RT_FindVariable(TOKEN tok);
void RT_CleanupVariables(void);
void RT_EnterScope(bool Private);
void RT_ExitScope(void);
void RT_MiniDebugPrint(VALUE *Value);
void RT_DebugPrint(size_t X, VALUE *Value, char *Name);
VALUE RT_RequireTypeIMPL(TYPE Type, const char *const func, const char *const file, int line);
void RT_Cleanup(void);
void RT_EmptyStack(void);
void RT_Initialise(void);
void RT_CleanupValue(VALUE *Value);
VALUE RT_Rel(int64_t rel);
bool RT_Is(VALUE *A, VALUE *B);
void RT_Append(VALUE *arr, VALUE val);
void RT_ArrayInit(VALUE *arr);
void RT_VisitParentScope(void);
void RT_VisitSubScope(void);
VALUE RT_Clone(VALUE value);

#endif
