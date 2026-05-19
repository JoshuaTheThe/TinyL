
#include <parse.h>

void parse_suffix(char **s)
{
        TOKEN tok = TK_Peek(s);
        switch (tok.Kind)
        {
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
                                parse_expr(s);
                                RT_CreateVariable(Value.as.function.arguments[argc++], RT_Pop());
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        parse_expr(&p);
                }
                else if (Value.Type == TYPE_BUILTIN)
                {
                        size_t argc = 0;
                        while (TK_Peek(s).Kind != TOKEN_RPAREN)
                        {
                                parse_expr(s);
                                argc++;
                        }
                        TK_Require(s, TOKEN_RPAREN);
                        Value.as.builtin(argc);
                }

                RT_ExitScope();
                break;
        }
        default: break;
        }
}

void parse_pri(char **s)
{
        TOKEN tok = TK_Peek(s);
        switch (tok.Kind)
        {
        case TOKEN_SUB: // negative expr prefix
        {
                TK_Next(s);
                parse_pri(s);
                VALUE Value = RT_RequireType(TYPE_INT);
                Value.as.integer = -Value.as.integer;
                RT_Push(Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_ADD: // positive expr prefix (invalid in this case, oh well)
        {
                TK_Next(s);
                parse_pri(s);
                VALUE Value = RT_RequireType(TYPE_INT);
                RT_Push(Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_NUMBER:
                TK_Next(s);
                RT_Push((VALUE){.Type=TYPE_INT, .as.integer = tok.Number});
                parse_suffix(s);
                break;
        case TOKEN_IF:
        {
                TK_Next(s);
                TK_Require(s, TOKEN_LPAREN);
                parse_expr(s);
                VALUE cond = RT_Pop();
                TK_Require(s, TOKEN_RPAREN);
                RT_EnterScope(false);
                TK_Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_INT && cond.as.integer > 0)
                {
                        parse_expr(s);
                }
                else
                {
                        TK_SkipBlock(s);
                        if (TK_Peek(s).Kind == TOKEN_ELSE)
                                RT_Push((VALUE){.Type=TYPE_NONE});
                        RT_ExitScope();
                        parse_pri(s);
                        break;
                }
                RT_ExitScope();
                parse_suffix(s);
                break;
        }

        case TOKEN_ELSE:
        {
                TK_Next(s);
                TK_Require(s, TOKEN_LPAREN);
                parse_expr(s);
                VALUE cond = RT_Pop();
                TK_Require(s, TOKEN_RPAREN);
                RT_EnterScope(false);
                TK_Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_NONE)
                {
                        parse_expr(s);
                }
                else
                {
                        TK_SkipBlock(s);
                }
                RT_ExitScope();
                parse_suffix(s);
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
                parse_suffix(s);
                break;
        }
        case TOKEN_DECLARE:
        {
                TK_Next(s);
                TOKEN name = TK_Require(s, TOKEN_IDENTIFIER);
                VARIABLE *Var = RT_CreateVariable(name, (VALUE){.Type=TYPE_NONE});
                if (TK_Peek(s).Kind == TOKEN_ASSIGN)
                {
                        TK_Next(s);
                        parse_expr(s);
                        Var->Value = RT_Pop();
                }

                RT_Push(Var->Value);
                break;
        }
        case TOKEN_IDENTIFIER:
        {
                TK_Next(s);
                VARIABLE *Var = RT_FindVariable(tok);
                if (TK_Peek(s).Kind == TOKEN_ASSIGN)
                {
                        TK_Next(s);
                        parse_expr(s);
                        Var->Value = RT_Pop();;
                }
                RT_Push(Var->Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_LPAREN: // grouped expression
        {
                RT_EnterScope(false);
                TK_Next(s);
                VALUE Value = {0};
                while (TK_Peek(s).Kind != TOKEN_RPAREN)
                {
                        parse_expr(s);
                        Value = RT_Pop();
                }
                TK_Next(s);
                RT_ExitScope();
                RT_Push(Value);
                parse_suffix(s);
                break;
        }
        default:
                TK_Next(s);
                break;
        }

        return;
}

void parse_mul(char **s)
{
        parse_pri(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_MUL || tok.Kind == TOKEN_DIV)
        {
                TK_Next(s);
                parse_pri(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_MUL)
                        RT_Push((VALUE){.as.integer = left.as.integer * right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_DIV)
                        RT_Push((VALUE){.as.integer = left.as.integer / right.as.integer, .Type = TYPE_INT});
                tok = TK_Peek(s);
        }
}

void parse_add(char **s)
{
        parse_mul(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_ADD || tok.Kind == TOKEN_SUB)
        {
                TK_Next(s);
                parse_mul(s);
                VALUE right = RT_RequireType(TYPE_INT);
                VALUE left = RT_RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_ADD)
                        RT_Push((VALUE){.as.integer = left.as.integer + right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_SUB)
                        RT_Push((VALUE){.as.integer = left.as.integer - right.as.integer, .Type = TYPE_INT});
                tok = TK_Peek(s);
        }
}

void parse_expr(char **s)
{
        parse_add(s);
        TOKEN tok = TK_Peek(s);
        while (tok.Kind == TOKEN_LSS || tok.Kind == TOKEN_GRT || tok.Kind == TOKEN_EQU)
        {
                TK_Next(s);
                parse_add(s);
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
        
        if (TK_Peek(s).Kind == TOKEN_EOE)
        {
                TK_Next(s);
                RT_EmptyStack();
        }
}
