#ifndef _AST_H_
#define _AST_H_

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
        long long  num;      // NODE_NUM
        char       var[64];  // NODE_VAR
        char       op;       // NODE_OP
    };
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

#endif