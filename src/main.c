
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
        TOKEN_EOF, // end of File
        TOKEN_EOE, // end of expression
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_LSS,
        TOKEN_GRT,
        TOKEN_EQU,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_NUMBER,
        TOKEN_IDENTIFIER,
        TOKEN_ASSIGN,
        TOKEN_DECLARE,
        TOKEN_FUNCTION,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_LSQ,
        TOKEN_RSQ,
} TOKENKIND;

typedef struct
{
        TOKENKIND Kind;
        char Identifier[16];
        uint64_t Number;
} TOKEN;

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


SCOPE Scopes[1024] = {0};
VALUE Stack[1024] = {0};
int Scope = 0;
int StackPointer = 0;

VALUE Pop(void)
{
        if (StackPointer > 0)
                return Stack[StackPointer--];
        return (VALUE){0};
}

void Push(VALUE Value)
{
        if (StackPointer < 1023)
        {
                Stack[++StackPointer] = Value;
                return;
        }

        printf("error: stack overflow\n");
}

VARIABLE *CreateVariable(TOKEN tok, VALUE Value)
{
        VARIABLE *Var = calloc(1, sizeof(*Var));
        memcpy(Var->Name, tok.Identifier, 16);
        Var->Next = Scopes[Scope].Variables;
        Var->Token = tok;
        Var->Value = Value;
        Scopes[Scope].Variables = Var;
        return Var;
}

VARIABLE *FindVariable(TOKEN tok)
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

void CleanUpVariables(void)
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

void EnterScope(bool Private)
{
        Scope++;
        Scopes[Scope].Private = Private;
        Scopes[Scope].Variables = NULL;
}

void ExitScope(void)
{
        CleanUpVariables();
        Scope--;
}

char GetChar(char **s)
{
        char chr = **s;
        *s += 1;
        return chr;
}

void DebugPrint(VALUE *Value, char *Name)
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

TOKEN NextToken(char **s)
{
        char chr;
        do
                chr = GetChar(s);
        while (chr == ' ' || chr == '\t' || chr == '\n');
        if (chr >= '0' && chr <= '9') // same for identifier, but dont convert to number
        {
                TOKEN Token = {.Kind = TOKEN_NUMBER, {0}, 0};
                for (; chr >= '0' && chr <= '9' && Token.Number < sizeof(Token.Identifier); ++Token.Number)
                {
                        Token.Identifier[Token.Number] = chr;
                        chr = GetChar(s);
                }

                *s -= 1;                               // we have read one too many characters
                Token.Number = atoi(Token.Identifier); // unsafe function, oh well
                return Token;
        }
        else if (chr >= 'a' && chr <= 'z')
        {
                TOKEN Token = {.Kind = TOKEN_IDENTIFIER, {0}, 0};
                for (; chr >= 'a' && chr <= 'z' && Token.Number < sizeof(Token.Identifier); ++Token.Number)
                {
                        Token.Identifier[Token.Number] = chr;
                        chr = GetChar(s);
                }

                *s -= 1;
                if (!strncmp(Token.Identifier, "let", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_DECLARE;
                }
                else if (!strncmp(Token.Identifier, "function", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_FUNCTION;
                }
                else if (!strncmp(Token.Identifier, "if", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_IF;
                }
                else if (!strncmp(Token.Identifier, "else", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_ELSE;
                }
                else if (!strncmp(Token.Identifier, "is", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_EQU;
                }
                return Token;
        }

        switch (chr)
        {
        case '+':
                return (TOKEN){.Kind = TOKEN_ADD, {0}, 0};
        case '-':
                return (TOKEN){.Kind = TOKEN_SUB, {0}, 0};
        case '*':
                return (TOKEN){.Kind = TOKEN_MUL, {0}, 0};
        case '/':
                return (TOKEN){.Kind = TOKEN_DIV, {0}, 0};
        case '<':
                return (TOKEN){.Kind = TOKEN_LSS, {0}, 0};
        case '>':
                return (TOKEN){.Kind = TOKEN_GRT, {0}, 0};
        case '(':
        case '{':
                return (TOKEN){.Kind = TOKEN_LPAREN, {0}, 0};
        case ')':
        case '}':
                return (TOKEN){.Kind = TOKEN_RPAREN, {0}, 0};
        case '=':
                return (TOKEN){.Kind = TOKEN_ASSIGN, {0}, 0};
        case ';':
                return (TOKEN){.Kind = TOKEN_EOE, {0}, 0};
        case '[':
                return (TOKEN){.Kind = TOKEN_LSQ, {0}, 0};
        case ']':
                return (TOKEN){.Kind = TOKEN_RSQ, {0}, 0};
        default:
                return (TOKEN){.Kind = TOKEN_EOF, {0}, 0};
        }
}

TOKEN PeekToken(char **s)
{
        char *temp = *s;
        return NextToken(&temp);
}

TOKEN Require(char **s, TOKENKIND Kind)
{
        TOKEN Next = NextToken(s);
        if (Next.Kind == Kind)
                return Next;
        printf("error: expected %d but got %d\n", Kind, Next.Kind);
        return (TOKEN){.Kind = TOKEN_EOF, {0}, 0};
}

VALUE RequireType(TYPE Type)
{
        VALUE Value = Pop();
        if (Value.Type == Type) return Value;
        printf("error: expected value of type %d but got %d\n", Type, Value.Type);
        return (VALUE){.Type=TYPE_NONE};
}

void parse_expr(char **s);

void parse_suffix(char **s)
{
        TOKEN tok = PeekToken(s);
        switch (tok.Kind)
        {
        case TOKEN_LPAREN: // call
        {
                EnterScope(false);
                NextToken(s);
                VALUE Value = Pop();
                if (Value.Type == TYPE_FN)
                {
                        char *p = Value.as.function.start;
                        size_t argc = 0;
                        while (PeekToken(s).Kind != TOKEN_RPAREN && argc < 16)
                        {
                                parse_expr(s);
                                CreateVariable(Value.as.function.arguments[argc++], Pop());
                        }
                        Require(s, TOKEN_RPAREN);
                        parse_expr(&p);
                }
                else if (Value.Type == TYPE_BUILTIN)
                {
                        size_t argc = 0;
                        while (PeekToken(s).Kind != TOKEN_RPAREN)
                        {
                                parse_expr(s);
                                argc++;
                        }
                        Require(s, TOKEN_RPAREN);
                        Value.as.builtin(argc);
                }

                ExitScope();
                break;
        }
        default: break;
        }
}

void SkipBlock(char **s)
{
        int depth = 1;
        while (depth > 0)
        {
                TOKEN tok = NextToken(s);
                if (tok.Kind == TOKEN_LPAREN)
                        depth++;
                if (tok.Kind == TOKEN_RPAREN)
                        depth--;
        }

        NextToken(s);
}

void parse_pri(char **s)
{
        TOKEN tok = PeekToken(s);
        switch (tok.Kind)
        {
        case TOKEN_SUB: // negative expr prefix
        {
                NextToken(s);
                parse_pri(s);
                VALUE Value = RequireType(TYPE_INT);
                Value.as.integer = -Value.as.integer;
                Push(Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_ADD: // positive expr prefix (invalid in this case, oh well)
        {
                NextToken(s);
                parse_pri(s);
                VALUE Value = RequireType(TYPE_INT);
                Push(Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_NUMBER:
                NextToken(s);
                Push((VALUE){.Type=TYPE_INT, .as.integer = tok.Number});
                parse_suffix(s);
                break;
        case TOKEN_IF:
        {
                NextToken(s);
                Require(s, TOKEN_LPAREN);
                parse_expr(s);
                VALUE cond = Pop();
                Require(s, TOKEN_RPAREN);
                EnterScope(false);
                Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_INT && cond.as.integer > 0)
                {
                        parse_expr(s);
                }
                else
                {
                        SkipBlock(s);
                        if (PeekToken(s).Kind == TOKEN_ELSE)
                                Push((VALUE){.Type=TYPE_NONE});
                        ExitScope();
                        parse_pri(s);
                        break;
                }
                ExitScope();
                parse_suffix(s);
                break;
        }

        case TOKEN_ELSE:
        {
                NextToken(s);
                Require(s, TOKEN_LPAREN);
                parse_expr(s);
                VALUE cond = Pop();
                Require(s, TOKEN_RPAREN);
                EnterScope(false);
                Require(s, TOKEN_LPAREN); // body

                if (cond.Type == TYPE_NONE)
                {
                        parse_expr(s);
                }
                else
                {
                        SkipBlock(s);
                }
                ExitScope();
                parse_suffix(s);
                break;
        }

        case TOKEN_FUNCTION:
        {
                NextToken(s);
                VALUE Value = {0};
                Value.Type = TYPE_FN;
                Require(s, TOKEN_LPAREN); // args, for now just none
                while (PeekToken(s).Kind != TOKEN_RPAREN && Value.as.function.argc < 16)
                {
                        TOKEN tok = NextToken(s);
                        Value.as.function.arguments[Value.as.function.argc++] = tok;
                }

                Require(s, TOKEN_RPAREN); // args, for now just none
                Value.as.function.start = *s;
                Require(s, TOKEN_LPAREN); // body
                SkipBlock(s);
                Push(Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_DECLARE:
        {
                NextToken(s);
                TOKEN name = Require(s, TOKEN_IDENTIFIER);
                VARIABLE *Var = CreateVariable(name, (VALUE){.Type=TYPE_NONE});
                if (PeekToken(s).Kind == TOKEN_ASSIGN)
                {
                        NextToken(s);
                        parse_expr(s);
                        Var->Value = Pop();
                }

                Push(Var->Value);
                break;
        }
        case TOKEN_IDENTIFIER:
        {
                NextToken(s);
                VARIABLE *Var = FindVariable(tok);
                if (PeekToken(s).Kind == TOKEN_ASSIGN)
                {
                        NextToken(s);
                        parse_expr(s);
                        Var->Value = Pop();;
                }
                Push(Var->Value);
                parse_suffix(s);
                break;
        }
        case TOKEN_LPAREN: // grouped expression
        {
                EnterScope(false);
                NextToken(s);
                VALUE Value = {0};
                while (PeekToken(s).Kind != TOKEN_RPAREN)
                {
                        parse_expr(s);
                        Value = Pop();
                }
                NextToken(s);
                ExitScope();
                Push(Value);
                parse_suffix(s);
                break;
        }
        default:
                NextToken(s);
                break;
        }

        return;
}

void parse_mul(char **s)
{
        parse_pri(s);
        TOKEN tok = PeekToken(s);
        while (tok.Kind == TOKEN_MUL || tok.Kind == TOKEN_DIV)
        {
                NextToken(s);
                parse_pri(s);
                VALUE right = RequireType(TYPE_INT);
                VALUE left = RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_MUL)
                        Push((VALUE){.as.integer = left.as.integer * right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_DIV)
                        Push((VALUE){.as.integer = left.as.integer / right.as.integer, .Type = TYPE_INT});
                tok = PeekToken(s);
        }
}

void parse_add(char **s)
{
        parse_mul(s);
        TOKEN tok = PeekToken(s);
        while (tok.Kind == TOKEN_ADD || tok.Kind == TOKEN_SUB)
        {
                NextToken(s);
                parse_mul(s);
                VALUE right = RequireType(TYPE_INT);
                VALUE left = RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_ADD)
                        Push((VALUE){.as.integer = left.as.integer + right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_SUB)
                        Push((VALUE){.as.integer = left.as.integer - right.as.integer, .Type = TYPE_INT});
                tok = PeekToken(s);
        }
}

void parse_expr(char **s)
{
        parse_add(s);
        TOKEN tok = PeekToken(s);
        while (tok.Kind == TOKEN_LSS || tok.Kind == TOKEN_GRT || tok.Kind == TOKEN_EQU)
        {
                NextToken(s);
                parse_add(s);
                VALUE right = RequireType(TYPE_INT);
                VALUE left = RequireType(TYPE_INT);
                if (tok.Kind == TOKEN_LSS)
                        Push((VALUE){.as.integer = left.as.integer < right.as.integer, .Type = TYPE_INT});
                else if (tok.Kind == TOKEN_GRT)
                        Push((VALUE){.as.integer = left.as.integer > right.as.integer, .Type = TYPE_INT});
                else
                        Push((VALUE){.as.integer = left.as.integer == right.as.integer, .Type = TYPE_INT});
                tok = PeekToken(s);
        }
}

int64_t execute(char *s)
{
        char *p = s;
        int64_t res = 0;
        while (PeekToken(&p).Kind != TOKEN_EOF)
        {
                parse_expr(&p);
                if (PeekToken(&p).Kind == TOKEN_EOE)
                {
                        NextToken(&p);
                        StackPointer = 0;
                }
        }

        return res;
}

int main(int argc, char **argv)
{
        if (argc != 2)
                return 1;
        FILE *f = fopen(argv[1], "rb");
        if (!f)
                return 1;
        fseek(f, 0, SEEK_END);
        size_t s = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buf = calloc(1, s + 1);
        fread(buf, 1, s, f);
        EnterScope(true);
        int res = execute(buf);
        free(buf);
        fclose(f);
        printf("<VARIABLES>\n");
        while (Scope > 0)
        {
                VARIABLE *Variable = Scopes[Scope].Variables;
                while (Variable)
                {
                        DebugPrint(&Variable->Value, Variable->Name);
                        Variable = Variable->Next;
                }
                
                ExitScope();
        }

        printf("<STACK>\n");
        while (StackPointer > 0)
        {
                VALUE Value = Pop();
                DebugPrint(&Value, "");
        }
        return res;
}
