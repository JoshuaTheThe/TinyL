
#include <rt.h>

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
        memcpy(Var->Name, tok.Identifier, 16);
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
                while (Var && memcmp(Var->Name, tok.Identifier, 16))
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

void RT_CleanupVariables(void)
{
        VARIABLE *Var = Scopes[Scope].Variables;
        while (Var)
        {
                VARIABLE *Prev = Var;
                Var = Var->Next;
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

void RT_DebugPrint(VALUE *Value, char *Name)
{
        if (Value->Type == TYPE_INT)
                printf("%.4x:%32s<i64>    =%ld\n", Scope, Name, Value->as.integer);
        else if (Value->Type == TYPE_FN)
                printf("%.4x:%32s<fn>      fn@%p\n", Scope, Name, (void*)Value->as.function.start);
        else if (Value->Type == TYPE_BUILTIN)
                printf("%.4x:%32s<builtin> builtin@%p\n", Scope, Name, Value->as.builtin);
        else if (Value->Type == TYPE_NONE)
                printf("%.4x:%32s<none>\n", Scope, Name);
}

VALUE RT_RequireType(TYPE Type)
{
        VALUE Value = RT_Pop();
        if (Value.Type == Type) return Value;
        printf("error: expected value of type %d but got %d\n", Type, Value.Type);
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
                        RT_DebugPrint(&Variable->Value, Variable->Name);
                        Variable = Variable->Next;
                }
                
                RT_ExitScope();
        }

        printf("<STACK>\n");
        while (StackPointer > 0)
        {
                VALUE Value = RT_Pop();
                RT_DebugPrint(&Value, "");
        }
}

void RT_EmptyStack(void)
{
        StackPointer = 0;
}
