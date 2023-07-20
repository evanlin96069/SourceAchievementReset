#include "memalloc.h"

#include "vcall.h"

__declspec(dllimport) void *g_pMemAlloc;

DECL_IFUNC(PUBLIC, void *, g_pMemAlloc, Alloc, 1, size_t);
DECL_IFUNC(PUBLIC, void *, g_pMemAlloc, Realloc, 3, void *, size_t);
DECL_IFUNC(PUBLIC, void, g_pMemAlloc, Free, 5, void *);
