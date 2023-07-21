#include "hook.h"

#include <windows.h>

void *HookVirtual(void **addr, void *target) {
    DWORD old;
    if (!VirtualProtect(addr, sizeof(void *), PAGE_READWRITE, &old)) {
        return NULL;
    }

    void *orig = *addr;
    *addr = target;
    return orig;
}
