
#include <parse.h>

void PRS_Suffix(char **s)
{
        TOKEN tok = TK_Peek(s);
        switch (tok.Kind)
        {
        case TOKEN_LSQ: // access
        {
                RT_EnterScope(false);
                TK_Next(s);
                VALUE Base = RT_RequireType(TYPE_ARRAY);
                while (TK_Peek(s).Kind != TOKEN_RSQ)
                {
                        PRS_Expression(s);
                }

                VALUE Index = RT_RequireType(TYPE_INT);
                VALUE Copy = Index.as.integer > 0 && Index.as.integer < Base.as.array.count ? Base.as.array.items[Index.as.integer] : (VALUE){.Type = TYPE_NONE};
                RT_CleanupValue(&Base);
                RT_Push(Copy);
                RT_ExitScope();
                break;
        }
        case TOKEN_LPAREN: // call
        {
                RT_EnterScope(false);
                TK_Next(s);
                VALUE Value = RT_Pop();
                if (Value.Type == TYPE_FN)
                {
                        char *p = Value.as.function.start;
                        size_t argc = 0;
                        while (TK_Peek(s).Kind != TOKEN_RPAREN && argc < 16)
                        {
                                PRS_Expression(s);
                                RT_CreateVariable(Value.as.function.arguments[argc++], RT_Pop());
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        PRS_Expression(&p);
                }
                else if (Value.Type == TYPE_BUILTIN)
                {
                        size_t argc = 0;
                        while (TK_Peek(s).Kind != TOKEN_RPAREN)
                        {
                                PRS_Expression(s);
                                argc++;
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        Value.as.builtin(argc);
                }

                RT_ExitScope();
                break;
        }
        default:
                break;
        }
}

void PRS_Primary(char **s)
{
        TOKEN tok = TK_Peek(s);
        switch (tok.Kind)
        {
        case TOKEN_SUB: // negative expr prefix
        {
                TK_Next(s);
                PRS_Primary(s);
                VALUE Value = RT_RequireType(TYPE_INT);
                Value.as.integer = -Value.as.integer;
                RT_Push(Value);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_ADD: // positive expr prefix (invalid in this case, oh well)
        {
                TK_Next(s);
                PRS_Primary(s);
                VALUE Value = RT_RequireType(TYPE_INT);
                RT_Push(Value);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_NUMBER:
                TK_Next(s);
                RT_Push((VALUE){.Type = TYPE_INT, .as.integer = tok.Number});
                PRS_Suffix(s);
                break;
        case TOKEN_LSQ:
        {
                TK_Next(s);
                VALUE arr = {.Type = TYPE_ARRAY};
                arr.as.array.count = 0;
                arr.as.array.capacity = 8;
                arr.as.array.items = malloc(sizeof(VALUE) * 8);
                while (TK_Peek(s).Kind != TOKEN_RSQ)
                {
                        PRS_Expression(s);
                        VALUE item = RT_Pop();
                        if (arr.as.array.count >= arr.as.array.capacity)
                        {
                                arr.as.array.capacity *= 2;
                                arr.as.array.items = realloc(arr.as.array.items,
                                                             sizeof(VALUE) * arr.as.array.capacity);
                        }

                        arr.as.array.items[arr.as.array.count++] = item;
                }

                TK_Require(s, TOKEN_RSQ);
                RT_Push(arr);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_IF:
        {
                TK_Next(s);
                TK_Require(s, TOKEN_LPAREN);
                PRS_Expression(s);
                VALUE cond = RT_Pop();
                TK_Require(s, TOKEN_RPAREN);
                RT_EnterScope(false);
                TK_Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_INT && cond.as.integer > 0)
                {
                        PRS_Expression(s);
                }
                else
                {
                        TK_SkipBlock(s);
                        if (TK_Peek(s).Kind == TOKEN_ELSE)
                        {
                                RT_Push((VALUE){.Type = TYPE_NONE});
                                RT_ExitScope();
                                PRS_Primary(s);
                                break;
                        }

                        RT_ExitScope();
                        PRS_Suffix(s);
                        break;
                }
                RT_ExitScope();
                PRS_Suffix(s);
                break;
        }
        case TOKEN_ELSE:
        {
                TK_Next(s);
                TK_Require(s, TOKEN_LPAREN);
                PRS_Expression(s);
                VALUE cond = RT_Pop();
                TK_Require(s, TOKEN_RPAREN);
                RT_EnterScope(false);
                TK_Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_NONE)
                {
                        PRS_Expression(s);
                }
                else
                {
                        TK_SkipBlock(s);
                }
                RT_ExitScope();
                PRS_Suffix(s);
                break;
        }

        case TOKEN_FUNCTION:
        {
                TK_Next(s);
                VALUE Value = {0};
                Value.Type = TYPE_FN;
                TK_Require(s, TOKEN_LPAREN); // args, for now just none
                while (TK_Peek(s).Kind != TOKEN_RPAREN && Value.as.function.argc < 16)
                {
                        TOKEN tok = TK_Next(s);
                        Value.as.function.arguments[Value.as.function.argc++] = tok;
                }

                TK_Require(s, TOKEN_RPAREN); // args, for now just none
                Value.as.function.start = *s;
                TK_Require(s, TOKEN_LPAREN); // body
                TK_SkipBlock(s);
                RT_Push(Value);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_DECLARE:
        {
                TK_Next(s);
                TOKEN name = TK_Require(s, TOKEN_IDENTIFIER);
                VARIABLE *Var = RT_CreateVariable(name, (VALUE){.Type = TYPE_NONE});
                if (TK_Peek(s).Kind == TOKEN_ASSIGN)
                {
                        TK_Next(s);
                        PRS_Expression(s);
                        Var->Value = RT_Pop();
                }

                RT_Push(Var->Value);
                break;
        }
        case TOKEN_IDENTIFIER:
        {
                TK_Next(s);
                VARIABLE *Var = RT_FindVariable(tok);
                TOKEN next = TK_Peek(s);

                if (next.Kind == TOKEN_ASSIGN || next.Kind == TOKEN_CREATE_ASSIGN)
                {
                        if (next.Kind == TOKEN_CREATE_ASSIGN)
                                Var = RT_CreateVariable(tok, (VALUE){.Type = TYPE_NONE});
                        else if (!Var)
                                printf("error: variable '%s' not declared\n", tok.Identifier);

                        TK_Next(s);
                        PRS_Expression(s);
                        if (Var)
                                Var->Value = RT_Pop();
                }

                if (Var)
                        RT_Push(Var->Value);
                else
                        RT_Push((VALUE){.Type = TYPE_NONE});

                PRS_Suffix(s);
                break;
        }
        case TOKEN_LPAREN: // grouped expression
        {
                RT_EnterScope(false);
                TK_Next(s);
                VALUE Value = {0};
                while (TK_Peek(s).Kind != TOKEN_RPAREN)
                {
                        PRS_Expression(s);
                        Value = RT_Pop();
                }
                TK_Next(s);
                RT_ExitScope();
                RT_Push(Value);
                PRS_Suffix(s);
                break;
        }
        default:
                TK_Next(s);
                break;
        }

        return;
}

void PRS_MulDiv(char **s)
{
        PRS_Primary(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_MUL || tok.Kind == TOKEN_DIV)
        {
                TK_Next(s);
                PRS_Primary(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_MUL)
                        RT_Push((VALUE){.as.integer = left.as.integer * right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_DIV)
                        RT_Push((VALUE){.as.integer = left.as.integer / right.as.integer, .Type = TYPE_INT});
                tok = TK_Peek(s);
        }
}

void PRS_AddSub(char **s)
{
        PRS_MulDiv(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_ADD || tok.Kind == TOKEN_SUB)
        {
                TK_Next(s);
                PRS_MulDiv(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_ADD)
                        RT_Push((VALUE){.as.integer = left.as.integer + right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_SUB)
                        RT_Push((VALUE){.as.integer = left.as.integer - right.as.integer, .Type = TYPE_INT});
                tok = TK_Peek(s);
        }
}

void PRS_Expression(char **s)
{
        PRS_AddSub(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_LSS || tok.Kind == TOKEN_GRT || tok.Kind == TOKEN_EQU)
        {
                TK_Next(s);
                PRS_AddSub(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_LSS)
                        RT_Push((VALUE){.as.integer = left.as.integer < right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_GRT)
                        RT_Push((VALUE){.as.integer = left.as.integer > right.as.integer, .Type = TYPE_INT});
                else
                        RT_Push((VALUE){.as.integer = left.as.integer == right.as.integer, .Type = TYPE_INT});
                tok = TK_Peek(s);
        }
}

void Execute(char *s)
{
        char *p = s;
        while (TK_Peek(&p).Kind != TOKEN_EOF)
        {
                PRS_Expression(&p);
                if (TK_Peek(&p).Kind == TOKEN_EOE)
                {
                        TK_Next(&p);
                        RT_EmptyStack();
                }
        }
}
