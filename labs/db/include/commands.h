#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stddef.h>
#include "types.h"

int gen(size_t, size_t, size_t);
void get_size(void);
void print_students(int);
char *get_attestation(attestation_type);

#endif
