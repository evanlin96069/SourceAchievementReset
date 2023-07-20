#ifndef HOOK_H
#define HOOK_H

#include <stdbool.h>

void *HookVirtual(void **addr, void *target);

/*
 * Puts an original function back after hooking.
 */
static inline void UnhookVirtual(void **addr, void *orig) { *addr = orig; }

bool HookInit(void);

/*
 * Returns a trampoline pointer, or null if hooking failed. Unlike
 * hook_vtable, handles memory protection for you.
 */
void *HookInline(void *func, void *target);

/*
 * Reverts the function to its original unhooked state. Takes the pointer to the
 * callable "original" function, i.e. the trampoline, NOT the initial function
 * pointer from before hooking.
 */
void UnhookInline(void *orig);

#endif
