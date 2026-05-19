#ifndef TOK_H
#define TOK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDENTIFIER_SIZE (256)

typedef enum
{
        TOKEN_EOF, // end of File
        TOKEN_EOE, // end of expression
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_MOD,
        TOKEN_LSS,
        TOKEN_GRT,
        TOKEN_EQU,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_NUMBER,
        TOKEN_IDENTIFIER,
        TOKEN_ASSIGN,
        TOKEN_CREATE_ASSIGN,
        TOKEN_DECLARE,
        TOKEN_FUNCTION,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_LSQ,
        TOKEN_RSQ,
        TOKEN_STRING,
} TOKENKIND;

typedef struct
{
        TOKENKIND Kind;
        char Identifier[IDENTIFIER_SIZE];
        uint64_t Number;
} TOKEN;

TOKEN TK_Next(char **s);
TOKEN TK_Peek(char **s);
TOKEN TK_Require(char **s, TOKENKIND Kind);
void TK_SkipBlock(char **s);

#endif
