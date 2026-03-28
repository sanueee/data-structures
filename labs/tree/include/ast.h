#ifndef _AST_H_
#define _AST_H_

#include <stdio.h>

typedef enum
{
    NODE_NUM,
    NODE_VAR,
    NODE_OP
} NodeKind;

typedef struct ASTNode
{
    NodeKind kind;
    union
    {
        long long num;
        char var[64];
        char op;
    };
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *ast_make_num(long long n);
ASTNode *ast_make_var(const char *name);
ASTNode *ast_make_op(char op, ASTNode *left, ASTNode *right);
void ast_free(ASTNode *node);
void ast_save_prf(ASTNode *node, FILE *out);
void ast_save_pst(ASTNode *node, FILE *out);

#endif
