#ifndef _EVAL_H_
#define _EVAL_H_

#include "ast.h"

typedef struct
{
    char name[64];
    long long value;
} VarBinding;

int ast_eval(ASTNode *node,
             VarBinding *bindings, int n_bindings,
             long long *result);

#endif
