#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "types.h"
#include <stdbool.h>

int add_student(person *);
size_t get_count(void);
person *get_student(size_t);
size_t get_total_size(void);
bool can_allocate(size_t, size_t);

void clean(void);

#endif
