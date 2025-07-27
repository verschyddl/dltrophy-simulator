#pragma once

// from fastled_progmem.h
#define FL_PROGMEM
#define FL_PGM_READ_BYTE_NEAR(x)  (*((const  uint8_t*)(x)))
#define FL_PGM_READ_WORD_NEAR(x)  (*((const uint16_t*)(x)))
#define FL_PGM_READ_DWORD_NEAR(x) (*((const uint32_t*)(x)))

// from fastled_config.h
#define FASTLED_SCALE8_FIXED 1
#define FASTLED_BLEND_FIXED 1
#define FASTLED_NOISE_FIXED 1
