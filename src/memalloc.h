#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stddef.h>

/*
 * These functions are just like malloc/realloc/free, but they call into
 * Valve's memory allocator wrapper, which ensures that allocations crossing
 * plugin/engine boundaries won't cause any weird issues.
 */

void* Alloc(size_t size);
void* Realloc(void* mem, size_t size);
void Free(void* mem);

#endif
