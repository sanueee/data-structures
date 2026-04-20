#ifndef _TYPES_H_
#define _TYPES_H_

typedef struct node
{
    int value;
    int height;
    int size;
    struct node *left;
    struct node *right;
} node;

#endif