#ifndef AVL_H
#define AVL_H

#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

node *rotate_right(node *);
node *rotate_left(node *);
node *balance(node *);

unsigned long long get_height(node *);
unsigned long long get_size(node *);

void update(node *);
node *init(int);

// kahooso
node *insert(node *, int);
node *delete_node(node *, int);
int find(node *, int);
void avl_print(node *);
node *kth(node *, unsigned long long);

// sanueee
unsigned long long range_count(node *, int, int);
int prev(node *, int, int *);
int next(node *, int, int *);

#endif
