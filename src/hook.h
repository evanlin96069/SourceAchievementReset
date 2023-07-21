#ifndef HOOK_H
#define HOOK_H

#define HOOK_VTABLE(vtable, offset) (&(*(void ***)(vtable))[(offset)])
#define HOOK_IFUNC(interface, func) (((void **)&(*(interface))->func))

void *HookVirtual(void **addr, void *target);
static inline void UnhookVirtual(void **addr, void *orig) { *addr = orig; }

#endif
