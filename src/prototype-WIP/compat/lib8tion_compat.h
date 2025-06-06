//
// Created by qm210 on 06.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_LIB8TION_COMPAT_H
#define DLTROPHY_SIMULATOR_LIB8TION_COMPAT_H

#include <cstdint>

typedef uint8_t   fract8;

/// Define a LIB8TION member function as static inline with an "unused" attribute
#define LIB8STATIC __attribute__ ((unused)) static inline
/// Define a LIB8TION member function as always static inline
#define LIB8STATIC_ALWAYS_INLINE __attribute__ ((always_inline)) static inline


LIB8STATIC_ALWAYS_INLINE uint8_t qadd8(uint8_t i, uint8_t j) {
    unsigned int t = i + j;
    return t > 255 ? 255 : t;
}

LIB8STATIC_ALWAYS_INLINE uint8_t qsub8(uint8_t i, uint8_t j) {
    int t = i - j;
    return t < 0 ? 0 : t;
}

LIB8STATIC_ALWAYS_INLINE uint8_t qmul8(uint8_t i, uint8_t j) {
    unsigned p = (unsigned)i * (unsigned)j;
    return p > 255 ? 255 : p;
}

// see FastLED.h, this is the default
#define FASTLED_SCALE8_FIXED 1

LIB8STATIC_ALWAYS_INLINE uint8_t scale8(uint8_t i, fract8 scale) {
#if (FASTLED_SCALE8_FIXED == 1)
    return (((uint16_t)i) * (1+(uint16_t)(scale))) >> 8;
#else
    return ((uint16_t)i * (uint16_t)(scale) ) >> 8;
#endif
}

LIB8STATIC void nscale8x3( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale) {
#if (FASTLED_SCALE8_FIXED == 1)
    uint16_t scale_fixed = scale + 1;
    r = (((uint16_t)r) * scale_fixed) >> 8;
    g = (((uint16_t)g) * scale_fixed) >> 8;
    b = (((uint16_t)b) * scale_fixed) >> 8;
#else
    r = ((int)r * (int)(scale) ) >> 8;
    g = ((int)g * (int)(scale) ) >> 8;
    b = ((int)b * (int)(scale) ) >> 8;
#endif
}

LIB8STATIC void nscale8x3_video( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale) {
    uint8_t nonzeroscale = (scale != 0) ? 1 : 0;
    r = (r == 0) ? 0 : (((int) r * (int) (scale)) >> 8) + nonzeroscale;
    g = (g == 0) ? 0 : (((int) g * (int) (scale)) >> 8) + nonzeroscale;
    b = (b == 0) ? 0 : (((int) b * (int) (scale)) >> 8) + nonzeroscale;
}

#endif //DLTROPHY_SIMULATOR_LIB8TION_COMPAT_H
