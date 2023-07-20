/*
 * Copyright © 2023 Michael Smith <mikesmiffy128@gmail.com>
 * Copyright © 2022 Willian Henrique <wsimanbrazil@yahoo.com.br>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "hook.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

#include "mem.h"
#include "x86.h"

void *HookVirtual(void **addr, void *target) {
    DWORD old;
    if (!VirtualProtect(addr, sizeof(void *), PAGE_READWRITE, &old)) {
        return NULL;
    }

    void *orig = *addr;
    *addr = target;
    return orig;
}

// Warning: half-arsed hacky implementation (because that's all we really need)
// Almost certainly breaks in some weird cases. Oh well! Most of the time,
// vtable hooking is more reliable, this is only for, uh, emergencies.

#if defined(_WIN32) && !defined(_WIN64)

#if defined(__GNUC__) || defined(__clang__)
__attribute__((aligned(4096)))
#elif defined(_MSC_VER)
__declspec(align(4096))
#else
#error no way to align stuff!
#endif
static uint8_t trampolines[4096];
static uint8_t *nexttrampoline = trampolines;

static inline bool os_mprot(void *addr, int len, int fl) {
    unsigned long old;
    return !!VirtualProtect(addr, len, fl, &old);
}

bool HookInit(void) {
    // PE doesn't support rwx sections, not sure about ELF. Meh, just set it
    // here instead.
    return os_mprot(trampolines, sizeof(trampolines), PAGE_EXECUTE_READWRITE);
}

void *HookInline(void *func_, void *target) {
    uint8_t *func = func_;
    // dumb hack: if we hit some thunk that immediately jumps elsewhere (which
    // seems common for win32 API functions), hook the underlying thing instead.
    while (*func == X86_JMPIW)
        func += mem_loadoffset(func + 1) + 5;
    if (!os_mprot(func, 5, PAGE_EXECUTE_READWRITE))
        return false;
    int len = 0;
    for (;;) {
        // FIXME: these cases may result in somewhat dodgy error messaging. They
        // shouldn't happen anyway though. Maybe if we're confident we just
        // compile 'em out of release builds some day, but that sounds a little
        // scary. For now preferring confusing messages over crashes, I guess.
        if (func[len] == X86_CALL) {
            // con_warn("hook_inline: can't trampoline call instructions\n");
            return 0;
        }
        int ilen = x86_len(func + len);
        if (ilen == -1) {
            // con_warn("hook_inline: unknown or invalid instruction\n");
            return 0;
        }
        len += ilen;
        if (len >= 5)
            break;
        if (func[len] == X86_JMPIW) {
            // con_warn("hook_inline: can't trampoline jmp instructions\n");
            return 0;
        }
    }
    // for simplicity, just bump alloc the trampoline. no need to free anyway
    if (nexttrampoline - trampolines > sizeof(trampolines) - len - 6)
        goto nosp;
    // TODO(opt): stop pretending to be thread-safe, it's just slowing us down
    uint8_t *trampoline = (uint8_t *)InterlockedExchangeAdd(
        (volatile long *)&nexttrampoline, len + 6);
    // avoid TOCTOU
    if (trampoline - trampolines > sizeof(trampolines) - len - 6) {
    nosp:
        // con_warn("hook_inline: out of trampoline space\n");
        return 0;
    }
    *trampoline++ = len;  // stick length in front for quicker unhooking
    memcpy(trampoline, func, len);
    trampoline[len] = X86_JMPIW;
    uint32_t diff = func - (trampoline + 5);  // goto the continuation
    memcpy(trampoline + len + 1, &diff, 4);
    uint8_t jmp[8];
    jmp[0] = X86_JMPIW;
    diff = (uint8_t *)target - (func + 5);  // goto the hook target
    memcpy(jmp + 1, &diff, 4);
    // pad with original bytes so we can do an 8-byte atomic write
    memcpy(jmp + 5, func + 5, 3);
    *(volatile uint64_t *)func =
        *(uint64_t *)jmp;  // (assuming function is aligned)
    // -1 is the current process, and it's a constant in the WDK, so it's
    // assumed we can safely avoid the useless GetCurrentProcess call
    FlushInstructionCache((void *)-1, func, len);
    return trampoline;
}

void UnhookInline(void *orig) {
    uint8_t *p = orig;
    int len = p[-1];
    int off = mem_load32(p + len + 1);
    uint8_t *q = p + off + 5;
    memcpy(q, p, 5);  // XXX: not atomic atm! (does any of it even need to be?)
    FlushInstructionCache((void *)-1, q, 5);
}

#else

// TODO(linux): Implement for Linux and/or x86_64 when needed...

#endif

// vi: sw=4 ts=4 noet tw=80 cc=80
