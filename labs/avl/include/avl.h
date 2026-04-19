#ifndef AVL_H
#define AVL_H

#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

node *rotate_right(node *);
node *rotate_left(node *);
node *balance(node *);

node *init(int);
void update(node *);
unsigned long long get_height(node *);
unsigned long long get_size(node *);

// kahooso
node *insert(node *, int);
node *delete_node(node *, int);
int find(node *, int);
void avl_print(node *);
node *kth(node *, unsigned long long int);

// sanueee
unsigned long long range_count(node *, int, int);
node *prev_node(node *, int);
node *next_node(node *, int);

#endif
