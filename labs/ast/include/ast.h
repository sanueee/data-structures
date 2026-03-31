#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum
{
    NODE_NUM,
    NODE_VAR,
    NODE_OP
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    union
    {
        int num;
        char var[64];
        char op;
    };
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *ast_make_num(int num);
ASTNode *ast_make_var(const char *name);
ASTNode *ast_make_op(const char op, ASTNode *left, ASTNode *right);
void ast_free(ASTNode *node);
void ast_save_prf(ASTNode *node, FILE *output);
void ast_save_pst(ASTNode *node, FILE *output);

#endif