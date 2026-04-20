#ifndef _AVL_H_
#define _AVL_H_

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

node *right(node *);
node *left(node *);
node *balance(node *);

unsigned long long get_height(node *);
unsigned long long get_size(node *);

void update(node *);
node *init(int);

// kahooso
node *insert(node *, int);
node *delete_node(node *, int);
node *find_min(node *);
int find(node *, int);
void print(node *);
node *kth(node *, int);

// sanueee
unsigned long long range_count(node *, int, int);
int prev(node *, int, int *);
int next(node *, int, int *);

#endif