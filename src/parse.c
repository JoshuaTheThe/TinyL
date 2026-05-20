
#include <parse.h>
#include <error.h>

void PRS_Suffix(char **s)
{
        TOKEN tok = TK_Peek(s);
        switch (tok.Kind)
        {
        case TOKEN_LSQ:
        {
                TK_Next(s);
                VALUE Index = PRS_Expression(s);
                TK_Require(s, TOKEN_RSQ);
                VALUE Base = RT_Pop();

                if (Base.Type != TYPE_ARRAY)
                {
                        printf("error: cannot index non-array\n");
                        RT_Push((VALUE){.Type = TYPE_NONE});
                        PRS_Suffix(s);
                        abort();
                        break;
                }

                if (Index.Type != TYPE_INT)
                {
                        printf("error: array index must be integer\n");
                        RT_Push((VALUE){.Type = TYPE_NONE});
                        PRS_Suffix(s);
                        abort();
                        break;
                }

                int64_t idx = Index.as.integer;
                if (TK_Peek(s).Kind == TOKEN_ASSIGN)
                {
                        TK_Next(s);
                        VALUE Value = PRS_Expression(s);
                        if (idx < 0 || idx >= (int64_t)Base.as.array.count)
                        {
                                printf("error: index %ld out of bounds (size %zu)\n",
                                       idx, Base.as.array.count);
                                RT_Push((VALUE){.Type = TYPE_NONE});
                                abort();
                        }
                        else
                        {
                                RT_CleanupValue(&Base.as.array.items[idx]);
                                Base.as.array.items[idx] = Value;
                        }
                }
                else
                {
                        if (idx < 0 || idx >= (int64_t)Base.as.array.count)
                        {
                                printf("error: index %ld out of bounds (size %zu)\n",
                                       idx, Base.as.array.count);
                                RT_Push((VALUE){.Type = TYPE_NONE});
                                abort();
                        }
                        else
                        {
                                RT_Push(Base.as.array.items[idx]);
                        }
                }

                PRS_Suffix(s);
                break;
        }
        case TOKEN_LPAREN: // call
        {
                VALUE Value = RT_Pop();
                VALUE Return = {0};
                TK_Next(s);
                if (Value.Type == TYPE_FN)
                {
                        char *p = Value.as.function.start;
                        size_t argc = 0;
                        RT_EnterScope(false);
                        while (TK_Peek(s).Kind != TOKEN_RPAREN && argc < 6)
                        {
                                VALUE Argument = PRS_Expression(s);
                                RT_CreateVariable(Value.as.function.arguments[argc++], Argument);
                                if (TK_Peek(s).Kind == TOKEN_COMMA)
                                        TK_Next(s);
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        Return = PRS_Expression(&p);
                        RT_ExitScope();
                }
                else if (Value.Type == TYPE_BUILTIN)
                {
                        size_t argc = 0;
                        RT_EnterScope(false);
                        while (TK_Peek(s).Kind != TOKEN_RPAREN)
                        {
                                RT_Push(PRS_Expression(s));
                                if (TK_Peek(s).Kind == TOKEN_COMMA)
                                        TK_Next(s);
                                argc++;
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        Value.as.builtin(argc);
                        Return = RT_Pop();
                        RT_ExitScope();
                }

                RT_Push(Return);
                PRS_Suffix(s);
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
        case TOKEN_STRING:
        {
                TK_Next(s);
                VALUE arr = {.Type = TYPE_ARRAY};
                arr.as.array.count = 0;
                arr.as.array.capacity = 8;
                arr.as.array.items = malloc(sizeof(VALUE) * 8);
                arr.as.array.is_string = true;
                for (size_t i = 0; i < tok.Number; ++i)
                {
                        VALUE item = (VALUE){.Type = TYPE_INT, .as.integer = tok.Identifier[i]};
                        if (arr.as.array.count >= arr.as.array.capacity)
                        {
                                arr.as.array.capacity *= 2;
                                arr.as.array.items = realloc(arr.as.array.items,
                                                             sizeof(VALUE) * arr.as.array.capacity);
                        }

                        arr.as.array.items[arr.as.array.count++] = item;
                }

                RT_Push(arr);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_SUB: // negative expr prefix
        {
                TK_Next(s);
                PRS_Primary(s);
                VALUE Value = RT_RequireType(TYPE_INT);
                Value.as.integer = -Value.as.integer;
                RT_Push(Value);
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
                arr.as.array.is_string = false;
                while (TK_Peek(s).Kind != TOKEN_RSQ)
                {
                        VALUE item = PRS_Expression(s);
                        if (TK_Peek(s).Kind == TOKEN_COMMA)
                                TK_Next(s);

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
        case TOKEN_WHILE:
        {
                char *temp = *s;
                VALUE ret = {0};
                while (true)
                {
                        TK_Next(s);
                        TK_Require(s, TOKEN_LPAREN);
                        VALUE cond = PRS_Expression(s);
                        TK_Require(s, TOKEN_RPAREN);

                        if (cond.Type == TYPE_INT && cond.as.integer > 0)
                        {
                                TK_Require(s, TOKEN_LPAREN); // body
                                while (TK_Peek(s).Kind != TOKEN_RPAREN)
                                {
                                        ret = PRS_Expression(s);
                                }
                                TK_Require(s, TOKEN_RPAREN);
                        }
                        else
                        {
                                TK_Require(s, TOKEN_LPAREN);
                                TK_SkipBlock(s);
                                RT_Push(ret);
                                return;
                        }
                        *s = temp;
                }

                RT_Push(ret);
                break;
        }
        case TOKEN_IF:
        {
                TK_Next(s);
                TK_Require(s, TOKEN_LPAREN);
                VALUE cond = PRS_Expression(s);
                TK_Require(s, TOKEN_RPAREN);
                VALUE ret = {0};
                if (cond.Type == TYPE_INT && cond.as.integer > 0)
                {
                        TK_Require(s, TOKEN_LPAREN); // body
                        while (TK_Peek(s).Kind != TOKEN_RPAREN)
                        {
                                ret = PRS_Expression(s);
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        RT_Push(ret);
                        if (TK_Peek(s).Kind != TOKEN_ELSE)
                        {
                                return;
                        }
                }
                else
                {
                        TK_Require(s, TOKEN_LPAREN); // body
                        TK_SkipBlock(s);
                        RT_Push((VALUE){.Type = TYPE_NONE});
                        if (TK_Peek(s).Kind != TOKEN_ELSE)
                        {
                                return;
                        }
                }

                goto ELSE;
        }
        case TOKEN_ELSE:
        {
ELSE:
                TK_Next(s);
                VALUE cond = RT_Pop();
                VALUE ret = cond;

                if (cond.Type == TYPE_NONE)
                {
                        TK_Require(s, TOKEN_LPAREN); // body
                        while (TK_Peek(s).Kind != TOKEN_RPAREN)
                        {
                                ret = PRS_Expression(s);
                        }
                        TK_Require(s, TOKEN_RPAREN);
                }
                else
                {
                        TK_Require(s, TOKEN_LPAREN); // body
                        TK_SkipBlock(s);
                }

                RT_Push(ret);
                break;
        }

        case TOKEN_FUNCTION:
        {
                TK_Next(s);
                VALUE Value = {0};
                Value.Type = TYPE_FN;
                TK_Require(s, TOKEN_LPAREN); // args, for now just none
                while (TK_Peek(s).Kind != TOKEN_RPAREN && Value.as.function.argc < 6)
                {
                        TOKEN tok = TK_Next(s);
                        Value.as.function.arguments[Value.as.function.argc++] = tok;
                        if (TK_Peek(s).Kind == TOKEN_COMMA)
                                TK_Next(s);
                }

                TK_Require(s, TOKEN_RPAREN);
                Value.as.function.start = *s;
                TK_Require(s, TOKEN_LPAREN);
                TK_SkipBlock(s);
                RT_Push(Value);
                PRS_Suffix(s);
                break;
        }
        case TOKEN_DECLARE:
        {
                TK_Next(s);
                TOKEN name = TK_Require(s, TOKEN_IDENTIFIER);
                RT_VisitParentScope();
                VARIABLE *Var = RT_CreateVariable(name, (VALUE){.Type = TYPE_NONE});
                RT_VisitSubScope();
                if (TK_Peek(s).Kind == TOKEN_ASSIGN)
                {
                        TK_Next(s);
                        Var->Value = PRS_Expression(s);
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
                        {
                                RT_VisitParentScope();
                                Var = RT_CreateVariable(tok, (VALUE){.Type = TYPE_NONE});
                                RT_VisitSubScope();
                        }
                        else if (!Var)
                        {
                                error("variable '%s' not declared", tok.Identifier);
                                return;
                        }

                        TK_Next(s);
                        Var->Value = PRS_Expression(s);
                        return;
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
                TK_Next(s);
                VALUE Value = {0};
                while (TK_Peek(s).Kind != TOKEN_RPAREN)
                {
                        Value = PRS_Expression(s);
                        if (TK_Peek(s).Kind == TOKEN_COMMA)
                                TK_Next(s);
                }
                TK_Require(s, TOKEN_RPAREN);
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
        while (tok.Kind == TOKEN_MUL || tok.Kind == TOKEN_DIV || tok.Kind == TOKEN_MOD)
        {
                TK_Next(s);
                PRS_Primary(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_MUL)
                        RT_Push((VALUE){.as.integer = left.as.integer * right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_DIV)
                        RT_Push((VALUE){.as.integer = left.as.integer / right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_MOD)
                        RT_Push((VALUE){.as.integer = left.as.integer % right.as.integer, .Type = TYPE_INT});
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

VALUE PRS_Expression(char **s)
{
        RT_EnterScope(false);
        PRS_AddSub(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_LSS || tok.Kind == TOKEN_GRT || tok.Kind == TOKEN_EQU)
        {
                TK_Next(s);
                PRS_AddSub(s);
                if (tok.Kind == TOKEN_LSS)
                {
                        VALUE right = RT_RequireType(TYPE_INT);
                        VALUE left = RT_RequireType(TYPE_INT);
                        RT_Push((VALUE){.as.integer = left.as.integer < right.as.integer, .Type = TYPE_INT});
                }
                else if (tok.Kind == TOKEN_GRT)
                {
                        VALUE right = RT_RequireType(TYPE_INT);
                        VALUE left = RT_RequireType(TYPE_INT);
                        RT_Push((VALUE){.as.integer = left.as.integer > right.as.integer, .Type = TYPE_INT});
                }
                else
                {
                        VALUE right = RT_Pop();
                        VALUE left = RT_Pop();
                        RT_Push(INT(RT_Is(&left, &right)));
                }
                tok = TK_Peek(s);
        }

        if (TK_Peek(s).Kind == TOKEN_EOE)
        {
                TK_Next(s);
        }

        VALUE Return = RT_Pop();
        RT_ExitScope();
        return Return;
}

void Execute(char *s)
{
        char *p = s;
        while (TK_Peek(&p).Kind != TOKEN_EOF)
        {
                VALUE val = PRS_Expression(&p);
                RT_CleanupValue(&val);
        }
}