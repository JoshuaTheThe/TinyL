
#include <tok.h>

char TK_GetChar(char **s)
{
        char chr = **s;
        *s += 1;
        return chr;
}

TOKEN TK_Next(char **s)
{
        char chr;
        do
                chr = TK_GetChar(s);
        while (chr == ' ' || chr == '\t' || chr == '\n');

        if (chr >= '0' && chr <= '9')
        {
                TOKEN Token = {.Kind = TOKEN_NUMBER, {0}, 0};
                for (; chr >= '0' && chr <= '9' && Token.Number < sizeof(Token.Identifier); ++Token.Number)
                {
                        Token.Identifier[Token.Number] = chr;
                        chr = TK_GetChar(s);
                }

                *s -= 1;
                Token.Number = atoi(Token.Identifier);
                return Token;
        }
        else if (chr >= 'a' && chr <= 'z')
        {
                TOKEN Token = {.Kind = TOKEN_IDENTIFIER, {0}, 0};
                for (; chr >= 'a' && chr <= 'z' && Token.Number < sizeof(Token.Identifier); ++Token.Number)
                {
                        Token.Identifier[Token.Number] = chr;
                        chr = TK_GetChar(s);
                }

                *s -= 1;
                if (!strncmp(Token.Identifier, "let", sizeof(Token.Identifier)))
                        Token.Kind = TOKEN_DECLARE;
                else if (!strncmp(Token.Identifier, "function", sizeof(Token.Identifier)))
                        Token.Kind = TOKEN_FUNCTION;
                else if (!strncmp(Token.Identifier, "if", sizeof(Token.Identifier)))
                        Token.Kind = TOKEN_IF;
                else if (!strncmp(Token.Identifier, "else", sizeof(Token.Identifier)))
                        Token.Kind = TOKEN_ELSE;
                else if (!strncmp(Token.Identifier, "is", sizeof(Token.Identifier)))
                        Token.Kind = TOKEN_EQU;
                else if (!strncmp(Token.Identifier, "true", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_NUMBER;
                        Token.Number = true;
                }
                else if (!strncmp(Token.Identifier, "false", sizeof(Token.Identifier)))
                {
                        Token.Kind = TOKEN_NUMBER;
                        Token.Number = false;
                }
                
                return Token;
        }
        else if (chr == '"')
        {
                TOKEN Token = {.Kind = TOKEN_STRING, {0}, 0};
                chr = TK_GetChar(s);
                while (chr != '"' && chr != '\0')
                {
                        if (chr == '\\')
                        {
                                chr = TK_GetChar(s);
                                switch (chr)
                                {
                                case 'n':
                                        chr = '\n';
                                        break;
                                case 't':
                                        chr = '\t';
                                        break;
                                case 'r':
                                        chr = '\r';
                                        break;
                                case '\\':
                                        chr = '\\';
                                        break;
                                case '"':
                                        chr = '"';
                                        break;
                                case '\'':
                                        chr = '\'';
                                        break;
                                default:
                                        printf("error: unknown escape sequence '\\%c'\n", chr);
                                        break;
                                }
                        }

                        if (Token.Number < sizeof(Token.Identifier) - 1)
                                Token.Identifier[Token.Number++] = chr;
                        chr = TK_GetChar(s);
                }

                if (chr != '"')
                        printf("error: unterminated string\n");

                Token.Identifier[Token.Number] = '\0';
                return Token;
        }
        else if (chr == ':')
        {
                if (TK_GetChar(s) == '=')
                        return (TOKEN){.Kind = TOKEN_CREATE_ASSIGN, {0}, 0};
                *s -= 1;
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
        case '%':
                return (TOKEN){.Kind = TOKEN_MOD, {0}, 0};
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

TOKEN TK_Peek(char **s)
{
        char *temp = *s;
        return TK_Next(&temp);
}

TOKEN TK_Require(char **s, TOKENKIND Kind)
{
        TOKEN Next = TK_Next(s);
        if (Next.Kind == Kind)
                return Next;
        printf("error: expected %d but got %d\n", Kind, Next.Kind);
        return (TOKEN){.Kind = TOKEN_EOF, {0}, 0};
}

void TK_SkipBlock(char **s)
{
        int depth = 1;
        while (depth > 0)
        {
                TOKEN tok = TK_Next(s);
                if (tok.Kind == TOKEN_LPAREN)
                        depth++;
                if (tok.Kind == TOKEN_RPAREN)
                        depth--;
        }
}

