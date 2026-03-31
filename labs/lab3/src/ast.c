#include "../include/ast.h"
#include "../include/memstat.h"

ASTNode *ast_make_num(int num)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (node == NULL)
    {
        return NULL;
    }
    node->type = NODE_NUM;
    node->num = num;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode *ast_make_var(const char *name)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = NODE_VAR;
    strncpy(node->var, name, 63);
    node->var[63] = '\0';
    node->left = NULL;
    node->right = NULL;
    return node; 
}

ASTNode *ast_make_op(const char op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = MALLOC(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = NODE_OP;
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

static void prf_helper(ASTNode *node, FILE *output, int *need_space)
{
    if (!node)
        return;
    if (*need_space)
        fputc(' ', output);
    *need_space = 1;

    switch (node->type)
    {
    case NODE_NUM:
        fprintf(output, "%d", node->num);
        break;
    case NODE_VAR:
        fprintf(output, "%s", node->var);
        break;
    case NODE_OP:
        fputc(node->op, output);
        break;
    }
    prf_helper(node->left, output, need_space);
    prf_helper(node->right, output, need_space);
}

static void pst_helper(ASTNode *node, FILE *output, int *need_space)
{
    if (!node)
        return;
    pst_helper(node->left, output, need_space);
    pst_helper(node->right, output, need_space);

    if (*need_space)
        fputc(' ', output);
    *need_space = 1;

    switch (node->type)
    {
    case NODE_NUM:
        fprintf(output, "%d", node->num);
        break;
    case NODE_VAR:
        fprintf(output, "%s", node->var);
        break;
    case NODE_OP:
        fputc(node->op, output);
        break;
    }
}

void ast_save_prf(ASTNode *node, FILE *output)
{
    int ns = 0;
    prf_helper(node, output, &ns);
    fputc('\n', output);
}

void ast_save_pst(ASTNode *node, FILE *output)
{
    int ns = 0;
    pst_helper(node, output, &ns);
    fputc('\n', output);
}