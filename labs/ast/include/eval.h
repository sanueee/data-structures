#ifndef EVAL_H
#define EVAL_H

#include "ast.h"

typedef struct
{
    char name[64];
    long long value;
} VarBinding;

int ast_eval(ASTNode *node, VarBinding *bindings,
             int n_bindings, long long *result);

#endif