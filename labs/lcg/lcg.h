#ifndef LCG_H
#define LCG_H

#include <stddef.h>

typedef unsigned long long ull;

void lcg_seed(ull a, ull c, ull m, ull x0);
ull  lcg_next(void);
ull  lcg_range(ull lo, ull hi);

#endif
