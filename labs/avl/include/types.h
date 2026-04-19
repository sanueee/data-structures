#ifndef _TYPES_H_
#define _TYPES_H_

typedef struct node
{
    int value;
    unsigned long long int height;
    unsigned long long int size;
    struct node *left;
    struct node *right;
} node;

#endif
