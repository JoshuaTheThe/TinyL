
#include <rt.h>
#include <builtin.h>

SCOPE Scopes[1024] = {0};
VALUE Stack[1024] = {0};
int Scope = 0;
int StackPointer = 0;

VALUE RT_Pop(void)
{
        if (StackPointer > 0)
                return Stack[StackPointer--];
        return (VALUE){0};
}

VALUE RT_Rel(int64_t rel)
{
        if (StackPointer+rel > 0)
                return Stack[StackPointer+rel];
        return (VALUE){0};
}

void RT_Push(VALUE Value)
{
        if (StackPointer < 1023)
        {
                Stack[++StackPointer] = Value;
                return;
        }

        printf("error: stack overflow\n");
}

VARIABLE *RT_CreateVariable(TOKEN tok, VALUE Value)
{
        VARIABLE *Var = calloc(1, sizeof(*Var));
        memcpy(Var->Name, tok.Identifier, IDENTIFIER_SIZE);
        Var->Next = Scopes[Scope].Variables;
        Var->Token = tok;
        Var->Value = Value;
        Scopes[Scope].Variables = Var;
        return Var;
}

VARIABLE *RT_FindVariable(TOKEN tok)
{
        for (int depth = Scope; depth >= 0; --depth)
        {
                VARIABLE *Var = Scopes[depth].Variables;
                while (Var && memcmp(Var->Name, tok.Identifier, IDENTIFIER_SIZE))
                {
                        Var = Var->Next;
                }

                if (Var)
                        return Var;
                if (Scopes[depth].Private)
                        break;
        }

        return NULL;
}

void RT_CleanupValue(VALUE *Value)
{
        if (Value->Type == TYPE_ARRAY)
        {
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        RT_CleanupValue(&Value->as.array.items[i]);
                }

                free(Value->as.array.items);
        }
}

void RT_CleanupVariables(void)
{
        VARIABLE *Var = Scopes[Scope].Variables;
        while (Var)
        {
                VARIABLE *Prev = Var;
                Var = Var->Next;
                RT_CleanupValue(&Prev->Value);
                free(Prev);
        }

        Scopes[Scope].Variables = NULL;
}

void RT_EnterScope(bool Private)
{
        Scope++;
        Scopes[Scope].Private = Private;
        Scopes[Scope].Variables = NULL;
}

void RT_ExitScope(void)
{
        RT_CleanupVariables();
        Scope--;
}

void RT_MiniDebugPrint(VALUE *Value)
{
        if (Value->Type == TYPE_INT)
                printf("%ld", Value->as.integer);
        else if (Value->Type == TYPE_FN)
                printf("fn@%p", (void*)Value->as.function.start);
        else if (Value->Type == TYPE_BUILTIN)
                printf("builtin@%p", (void *)Value->as.builtin);
        else if (Value->Type == TYPE_ARRAY && !Value->as.array.is_string)
        {
                printf("[");
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        RT_MiniDebugPrint(&Value->as.array.items[i]);
                        if (i < Value->as.array.count - 1) printf(", ");
                }

                printf("]");
        }
        else if (Value->Type == TYPE_ARRAY && Value->as.array.is_string)
        {
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        printf("%c", Value->as.array.items[i].as.integer);
                }
        }
        else if (Value->Type == TYPE_NONE)
                printf("None");
}

void RT_DebugPrint(size_t X, VALUE *Value, char *Name)
{
        if (Value->Type == TYPE_INT)
                printf("%.4lx:%32s<i64>    =%ld\n", X, Name, Value->as.integer);
        else if (Value->Type == TYPE_FN)
                printf("%.4lx:%32s<fn>      fn@%p\n", X, Name, (void*)Value->as.function.start);
        else if (Value->Type == TYPE_BUILTIN)
                printf("%.4lx:%32s<builtin> builtin@%p\n", X, Name, (void *)Value->as.builtin);
        else if (Value->Type == TYPE_ARRAY && !Value->as.array.is_string)
        {
                printf("%.4lx:%32s<array>  =[", X, Name);
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        RT_MiniDebugPrint(&Value->as.array.items[i]);
                        if (i < Value->as.array.count - 1) printf(", ");
                }

                printf("]\n");
        }
        else if (Value->Type == TYPE_ARRAY && Value->as.array.is_string)
        {
                printf("%.4lx:%32s<string> =[", X, Name);
                printf("'");
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        printf("%c", Value->as.array.items[i].as.integer);
                }

                printf("'\n");
        }
        else if (Value->Type == TYPE_NONE)
                printf("%.4lx:%32s<none>\n", X, Name);
}

VALUE RT_RequireTypeIMPL(TYPE Type, const char *const func, const char *const file, int line)
{
        VALUE Value = RT_Pop();
        if (Value.Type == Type) return Value;
        printf("error: expected value of type %d but got %d, raised in %s in %s:%d\n", Type, Value.Type, func, file, line);
        return (VALUE){.Type=TYPE_NONE};
}

void RT_Cleanup(void)
{
        printf("<VARIABLES>\n");
        while (Scope > 0)
        {
                VARIABLE *Variable = Scopes[Scope].Variables;
                while (Variable)
                {
                        RT_DebugPrint(Scope, &Variable->Value, Variable->Name);
                        Variable = Variable->Next;
                }
                
                RT_ExitScope();
        }

        printf("<STACK>\n");
        while (StackPointer > 0)
        {
                VALUE Value = RT_Pop();
                RT_DebugPrint(StackPointer, &Value, "");
        }
}

void RT_EmptyStack(void)
{
        StackPointer = 0;
}

void RT_Initialise(void)
{
        RT_EmptyStack();
        RT_EnterScope(true);
        Builtin_AddBuiltinFunctions();
}
