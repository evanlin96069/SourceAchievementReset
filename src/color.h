#ifndef COLOR_H
#define COLOR_H

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
