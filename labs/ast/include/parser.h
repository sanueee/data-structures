#ifndef PARSER_H
#define PARSER_H

#include "../include/ast.h"

typedef enum
{
    TOK_EOF,
    TOK_NUM,
    TOK_VAR,
    TOK_OP,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_ERROR
} TokType;

typedef struct
{
    TokType type;
    int num;
    char var[64];
    char op;
} Token;

typedef struct
{
    const char *src;
    int pos;
    int error;
} Lexer;

ASTNode *parse_expr(const char *expr);
ASTNode *parse_prefix(const char *expr);
ASTNode *parse_postfix(const char *expr);

#endif