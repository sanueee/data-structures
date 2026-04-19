#ifndef TYPES_H
#define TYPES_H

typedef struct node
{
    int value;
    unsigned long long height;
    unsigned long long size;
    struct node *left;
    struct node *right;
} node;

#endif
