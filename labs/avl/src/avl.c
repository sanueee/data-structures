#include "../include/avl.h"

node *right(node *n)
{
    node *temp = n->left;
    n->left = temp->right;
    temp->right = n;
    update(n);
    update(temp);
    return temp;
}

node *left(node *n)
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
            n->left = left(n->left);
        return right(n);
    }

    if (bf == -2)
    {
        if (get_height(n->right->right) < get_height(n->right->left))
            n->right = right(n->right);
        return left(n);
    }

    return n;
}

unsigned long long get_height(node *n)
{
    return n ? n->height : 0;
}

unsigned long long get_size(node *n)
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

node *delete_node(node *n, int x)
{
    if (!n)
        return NULL;
    if (x < n->value)
    {
        n->left = delete_node(n->left, x);
    }
    else if (x > n->value)
    {
        n->right = delete_node(n->right, x);
    }
    else // x == n->value
    {
        if (!n->left && !n->right)
        {
            free(n);
            return NULL;
        }
        if (!n->left || !n->right)
        {
            node *child = n->left ? n->left : n->right;
            free(n);
            return child;
        }
        if (n->left && n->right)
        {
            node *min = find_min(n->right);
            n->value = min->value;
            n->right = delete_node(n->right, n->value);
        }
    }
    return balance(n);
}

node *find_min(node *n)
{
    if (!n)
        return NULL;
    while (n->left != NULL)
        n = n->left;
    return n;
}

int find(node *n, int x)
{
    if (!n)
        return -1;
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

void print(node *n)
{
    if (!n)
        return;
    print(n->left);
    printf("%d ", n->value);
    print(n->right);
    return;
}

node *kth(node *n, int k)
{
    if (!n)
        return NULL;
    int l = n->left ? n->left->size : 0;
    if (k == l)
        return n;
    if (k < l)
        return kth(n->left, k);
    else
        return kth(n->right, k - l - 1);
}

unsigned long long range_count(node *n, int l, int r)
{
    if (!n)
        return 0;
    if (n->value < l)
        return range_count(n->right, l, r);
    if (n->value > r)
        return range_count(n->left, l, r);
    return 1 + range_count(n->left, l, r) + range_count(n->right, l, r);
}

int prev(node *n, int x, int *result)
{
    if (!n)
        return -1;
    if (n->value < x)
    {
        *result = n->value;
        int found = prev(n->right, x, result);
        return 1;
    }
    else
        return prev(n->left, x, result);
}

int next(node *n, int x, int *result)
{
    if (!n)
        return 0;
    if (n->value > x)
    {
        *result = n->value;
        int found = next(n->left, x, result);
        return 1;
    }
    else
        return next(n->right, x, result);
}