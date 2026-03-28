#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ast.h"
#include "../include/memstat.h"

ASTNode *ast_make_num(long long n)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->kind = NODE_NUM;
    node->num = n;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode *ast_make_var(const char *name)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->kind = NODE_VAR;
    strncpy(node->var, name, 63);
    node->var[63] = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode *ast_make_op(char op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->kind = NODE_OP;
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

void ast_free(ASTNode *node)
{
    if (!node)
        return;
    ast_free(node->left);
    ast_free(node->right);
    FREE(node);
}

static void prf_helper(ASTNode *node, FILE *out, int *need_space)
{
    if (!node)
        return;
    if (*need_space)
        fputc(' ', out);
    *need_space = 1;

    switch (node->kind)
    {
    case NODE_NUM:
        fprintf(out, "%lld", node->num);
        break;
    case NODE_VAR:
        fprintf(out, "%s", node->var);
        break;
    case NODE_OP:
        fputc(node->op, out);
        break;
    }
    prf_helper(node->left, out, need_space);
    prf_helper(node->right, out, need_space);
}

static void pst_helper(ASTNode *node, FILE *out, int *need_space)
{
    if (!node)
        return;
    pst_helper(node->left, out, need_space);
    pst_helper(node->right, out, need_space);

    if (*need_space)
        fputc(' ', out);
    *need_space = 1;

    switch (node->kind)
    {
    case NODE_NUM:
        fprintf(out, "%lld", node->num);
        break;
    case NODE_VAR:
        fprintf(out, "%s", node->var);
        break;
    case NODE_OP:
        fputc(node->op, out);
        break;
    }
}

void ast_save_prf(ASTNode *node, FILE *out)
{
    int ns = 0;
    prf_helper(node, out, &ns);
    fputc('\n', out);
}

void ast_save_pst(ASTNode *node, FILE *out)
{
    int ns = 0;
    pst_helper(node, out, &ns);
    fputc('\n', out);
}
