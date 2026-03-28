#include <string.h>
#include "../include/ast.h"
#include "../include/eval.h"

static long long gcd(long long a, long long b)
{
    while (b)
    {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static long long lcm(long long a, long long b)
{
    return (a / gcd(a, b)) * b;
}

int ast_eval(ASTNode *node,
             VarBinding *bindings, int n_bindings,
             long long *result)
{
    if (!node)
        return 0;

    if (node->kind == NODE_NUM)
    {
        *result = node->num;
        return 1;
    }

    if (node->kind == NODE_VAR)
    {
        for (int i = 0; i < n_bindings; i++)
        {
            if (strcmp(bindings[i].name, node->var) == 0)
            {
                *result = bindings[i].value;
                return 1;
            }
        }
        return 0;
    }

    long long lv, rv;
    if (!ast_eval(node->left, bindings, n_bindings, &lv))
        return 0;
    if (!ast_eval(node->right, bindings, n_bindings, &rv))
        return 0;

    switch (node->op)
    {
    case '+':
        *result = lv + rv;
        break;
    case '-':
        *result = lv - rv;
        break;
    case '*':
        *result = lv * rv;
        break;
    case '/':
        if (rv == 0)
            return 0;
        *result = lv / rv;
        break;
    case '%':
        if (rv == 0)
            return 0;
        *result = lv % rv;
        break;
    case '#':
        if (lv <= 0 || rv <= 0)
            return 0;
        *result = gcd(lv, rv);
        break;
    case '@':
        if (lv <= 0 || rv <= 0)
            return 0;
        *result = lcm(lv, rv);
        break;
    default:
        return 0;
    }
    return 1;
}
