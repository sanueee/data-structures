#ifndef _MEMSTAT_H_
#define _MEMSTAT_H_

#include "types.h"

typedef struct
{
    size_t malloc_count;
    size_t calloc_count;
    size_t realloc_count;
    size_t free;
} memstat;

extern memstat stats;

#define MALLOC(size)       (memstat.malloc_count++,  malloc(size))
#define CALLOC(n, size)    (memstat.calloc_count++,  calloc(n, size))
#define FREE(ptr)          (memstat.free_count++,    free(ptr))
#define REALLOC(ptr, size) ((ptr) == NULL \
    ? (memstat.malloc_count++,  realloc(NULL, size)) \
    : (memstat.realloc_count++, realloc(ptr,  size)))

#endif