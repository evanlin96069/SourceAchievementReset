#ifndef HOOK_H
#define HOOK_H

#include <windows.h>

static inline bool Unprotect(void *addr) {
    DWORD old;
    return !!VirtualProtect(addr, sizeof(void *), PAGE_READWRITE, &old);
}

#endif
