#include "lcg.h"
#include <assert.h>

static ull g_a = 1664525ULL;
static ull g_c = 1013904223ULL;
static ull g_m = 4294967296ULL;
static ull g_x = 1ULL;

void lcg_seed(ull a, ull c, ull m, ull x0) {
    assert(m > 0);
    g_a = a;
    g_c = c;
    g_m = m;
    g_x = x0 % m;
}

ull lcg_next(void) {
    g_x = (g_a * g_x + g_c) % g_m;
    return g_x;
}

ull lcg_range(ull lo, ull hi) {
    if (hi <= lo) return lo;
    return lo + lcg_next() % (hi - lo + 1);
}
