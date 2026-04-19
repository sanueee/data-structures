#include "../include/avl.h"

node *rotate_right(node *n)
{
    node *temp = n->left;
    n->left = temp->right;
    temp->right = n;
    update(n);
    update(temp);
    return temp;
}

node *rotate_left(node *n)
{
    node *temp = n->right;
    n->right = temp->left;
    temp->left = n;
    update(n);
    update(temp);
    return temp;
}

node *balance(node *n)
{
    update(n);

    int bf = get_height(n->left) - get_height(n->right);

    if (bf == 2)
    {
        if (get_height(n->left->left) < get_height(n->left->right))
            n->left = rotate_left(n->left);
        return rotate_right(n);
    }

    if (bf == -2)
    {
        if (get_height(n->right->right) < get_height(n->right->left))
            n->right = rotate_right(n->right);
        return rotate_left(n);
    }

    return n;
}

unsigned long long get_height(node *n)
{
    return n ? n->height : 0;
}

unsigned long long int get_size(node *n)
{
    return n ? n->size : 0;
}

void update(node *n)
{
    if (!n)
        return;
    int lh = get_height(n->left);
    int rh = get_height(n->right);
    n->height = (lh > rh ? lh : rh) + 1;
    n->size = get_size(n->left) + get_size(n->right) + 1;
}

node *init(int x)
{
    node *n = (node *)malloc(sizeof(node));
    n->value = x;
    n->height = 0;
    n->size = 1;
    n->left = NULL;
    n->right = NULL;
    return n;
}

node *insert(node *n, int x)
{
    if (!n)
        return init(x);
    if (x == n->value)
        return n;
    if (x < n->value)
        n->left = insert(n->left, x);
    else
        n->right = insert(n->right, x);
    return balance(n);
}

int find(node *n, int x)
{
    while (n)
    {
        if (x == n->value)
            return 1;
        if (x < n->value)
            n = n->left;
        else
            n = n->right;
    }
    return 0;
}
