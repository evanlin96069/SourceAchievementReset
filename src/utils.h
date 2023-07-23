#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/*
 * Me: Can we have template?
 * C: We have template at home
 * template at home:
 */

#define CUtlMemory(T)         \
    struct CUtlMemory_##T {   \
        T* memory;            \
        int allocation_count; \
        int grow_size;        \
    }

#define CUtlVector(T)         \
    struct CUtlVector_##T {   \
        CUtlMemory(T) memory; \
        int size;             \
        T* element;           \
    }

// Some random type definitions

typedef struct Color {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t val;
        uint8_t bytes[4];
    };
} Color;

#endif
