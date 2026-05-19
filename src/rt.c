
#include <rt.h>
#include <builtin.h>

SCOPE Scopes[MAX_SCOPE_DEPTH] = {0};
int Scope = 0;

VALUE RT_Pop(void)
{
        if (Scopes[Scope].StackPointer > 0)
                return Scopes[Scope].Stack[Scopes[Scope].StackPointer--];
        return (VALUE){0};
}

VALUE RT_Rel(int64_t rel)
{
        if (Scopes[Scope].StackPointer + rel > 0)
                return Scopes[Scope].Stack[Scopes[Scope].StackPointer + rel];
        return (VALUE){0};
}

void RT_Push(VALUE Value)
{
        if (Scopes[Scope].StackPointer < MAX_STACK_DEPTH)
        {
                Scopes[Scope].Stack[++Scopes[Scope].StackPointer] = Value;
                return;
        }

        printf("error: stack overflow\n");
        printf("<STACK>\n");
        while (Scopes[Scope].StackPointer > 0 && Scopes[Scope].Stack)
        {
                VALUE Value = RT_Pop();
                RT_DebugPrint(Scopes[Scope].StackPointer, &Value, "");
        }
        abort();
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
        if (++Scope < MAX_SCOPE_DEPTH)
        {
                Scopes[Scope].Private = Private;
                Scopes[Scope].Variables = NULL;
                Scopes[Scope].StackPointer = 0;
                Scopes[Scope].Stack = calloc(MAX_STACK_DEPTH, sizeof(VALUE));
                return;
        }

        printf("error: scope overflow\n");
}

void RT_ExitScope(void)
{
        if (Scope > 0)
        {
                RT_CleanupVariables();
                while (Scopes[Scope].StackPointer > 0)
                {
                        RT_CleanupValue(&Scopes[Scope].Stack[Scopes[Scope].StackPointer]);
                        Scopes[Scope].StackPointer--;
                }
                
                free(Scopes[Scope].Stack);
                Scopes[Scope].Stack = NULL;
                Scope--;
        }
}

void RT_MiniDebugPrint(VALUE *Value)
{
        if (Value->Type == TYPE_INT)
                printf("%ld", Value->as.integer);
        else if (Value->Type == TYPE_FN)
                printf("fn@%p", (void *)Value->as.function.start);
        else if (Value->Type == TYPE_BUILTIN)
                printf("builtin@%p", (void *)Value->as.builtin);
        else if (Value->Type == TYPE_ARRAY && !Value->as.array.is_string)
        {
                printf("[");
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        RT_MiniDebugPrint(&Value->as.array.items[i]);
                        if (i < Value->as.array.count - 1)
                                printf(", ");
                }

                printf("]");
        }
        else if (Value->Type == TYPE_ARRAY && Value->as.array.is_string)
        {
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        printf("%c", (char)Value->as.array.items[i].as.integer);
                }
        }
        else if (Value->Type == TYPE_NONE)
                printf("None");
}

bool RT_Is(VALUE *A, VALUE *B)
{
        if (A->Type != B->Type)
                return false;

        switch (A->Type)
        {
        case TYPE_NONE:
                return true;
        case TYPE_INT:
                return A->as.integer == B->as.integer;
        case TYPE_FN:
                return (A->as.function.start == B->as.function.start &&
                        A->as.function.argc == B->as.function.argc &&
                        !memcmp(A->as.function.arguments, B->as.function.arguments,
                                sizeof(TOKEN) * A->as.function.argc));
        case TYPE_BUILTIN:
                return A->as.builtin == B->as.builtin;
        case TYPE_ARRAY:
                if (A->as.array.count != B->as.array.count)
                        return false;
                for (size_t i = 0; i < A->as.array.count; i++)
                {
                        if (!RT_Is(&A->as.array.items[i], &B->as.array.items[i]))
                                return false;
                }
                return true;
        default:
                return false;
        }
}

void RT_DebugPrint(size_t X, VALUE *Value, char *Name)
{
        if (Value->Type == TYPE_INT)
                printf("%.4lx:%32s<i64>    =%ld\n", X, Name, Value->as.integer);
        else if (Value->Type == TYPE_FN)
                printf("%.4lx:%32s<fn>      fn@%p\n", X, Name, (void *)Value->as.function.start);
        else if (Value->Type == TYPE_BUILTIN)
                printf("%.4lx:%32s<builtin> builtin@%p\n", X, Name, (void *)Value->as.builtin);
        else if (Value->Type == TYPE_ARRAY && !Value->as.array.is_string)
        {
                printf("%.4lx:%32s<array>  =[", X, Name);
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        RT_MiniDebugPrint(&Value->as.array.items[i]);
                        if (i < Value->as.array.count - 1)
                                printf(", ");
                }

                printf("]\n");
        }
        else if (Value->Type == TYPE_ARRAY && Value->as.array.is_string)
        {
                printf("%.4lx:%32s<string> =", X, Name);
                printf("'");
                for (size_t i = 0; i < Value->as.array.count; ++i)
                {
                        printf("%c", (char)Value->as.array.items[i].as.integer);
                }

                printf("'\n");
        }
        else if (Value->Type == TYPE_NONE)
                printf("%.4lx:%32s<none>\n", X, Name);
}

VALUE RT_RequireTypeIMPL(TYPE Type, const char *const func, const char *const file, int line)
{
        VALUE Value = RT_Pop();
        if (Value.Type == Type)
                return Value;
        printf("error: expected value of type %d but got %d, raised in %s in %s:%d\n", Type, Value.Type, func, file, line);
        return (VALUE){.Type = TYPE_NONE};
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

                printf("<STACK>\n");
                while (Scopes[Scope].StackPointer > 0 && Scopes[Scope].Stack)
                {
                        VALUE Value = RT_Pop();
                        RT_DebugPrint(Scopes[Scope].StackPointer, &Value, "");
                }
                RT_ExitScope();
        }
}

void RT_EmptyStack(void)
{
        Scopes[Scope].StackPointer = 0;
}

void RT_Initialise(void)
{
        RT_EmptyStack();
        RT_EnterScope(true);
        Builtin_AddBuiltinFunctions();
}

void RT_Append(VALUE *arr, VALUE val)
{
        if (arr->Type != TYPE_ARRAY)
        {
                printf("error: cannot append to non-array\n");
                return;
        }

        if (arr->as.array.count >= arr->as.array.capacity)
        {
                size_t new_cap = arr->as.array.capacity ? arr->as.array.capacity * 2 : 8;
                VALUE *new_items = realloc(arr->as.array.items, sizeof(VALUE) * new_cap);
                if (!new_items)
                {
                        printf("error: failed to grow array\n");
                        return;
                }
                arr->as.array.items = new_items;
                arr->as.array.capacity = new_cap;
        }

        arr->as.array.items[arr->as.array.count++] = val;
        if (arr->as.array.is_string && val.Type != TYPE_INT)
                arr->as.array.is_string = false;
}

void RT_ArrayInit(VALUE *arr)
{
        arr->Type = TYPE_ARRAY;
        arr->as.array.items = NULL;
        arr->as.array.count = 0;
        arr->as.array.capacity = 0;
        arr->as.array.is_string = false;
}
