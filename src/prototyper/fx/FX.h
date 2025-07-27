#pragma once

#include "lib/pixeltypes.h"
#include "lib/hsv2rgb.h"
#include "lib/colorutils.h"
#include "fx/wled/const.h"

/*
 * This file is a mock / shim so that DeadlineTrophy.h can work as expected
 * even though it was just brainlessly copied from our current WLED fork.
 */

// wled_math.cpp
int16_t sin16_t(uint16_t theta);
int16_t cos16_t(uint16_t theta);
uint8_t sin8_t(uint8_t theta);
uint8_t cos8_t(uint8_t theta);
float sin_approx(float theta); // uses integer math (converted to float), accuracy +/-0.0015 (compared to sinf())
float cos_approx(float theta);
float tan_approx(float x);
float atan2_t(float y, float x);
float acos_t(float x);
float asin_t(float x);
template <typename T> T atan_t(T x);
float floor_t(float x);
float fmod_t(float num, float denom);
uint32_t sqrt32_bw(uint32_t x);
#define sin_t sin_approx
#define cos_t cos_approx
#define tan_t tan_approx


#define RGBW32(r,g,b,w) (uint32_t((byte(w) << 24) | (byte(r) << 16) | (byte(g) << 8) | (byte(b))))
#define R(c) (byte((c) >> 16))
#define G(c) (byte((c) >> 8))
#define B(c) (byte(c))
#define W(c) (byte((c) >> 24))

/*
 * TO-MAYBE-DO qm210:
 * if something (e.g. some define or WLED-global constant) is missing:
 * first see whether we can put it here,
 * and if not, do something incomprehensibly smart.
 */

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define WLED_FPS         42
#define FRAMETIME_FIXED  (1000/WLED_FPS)
#define FRAMETIME        strip.getFrameTime()

#define MIN_SHOW_DELAY   (_frametime < 16 ? 8 : 15)

#define NUM_COLORS       3 /* number of colors per segment */
#define SEGMENT          (*strip._currentSegment)
#define SEGENV           (*strip._currentSegment)
#define SEGCOLOR(x)      Segment::getCurrentColor(x)
#define SEGPALETTE       Segment::getCurrentPalette()
#define SEGLEN           Segment::vLength()
#define SEG_W            Segment::vWidth()
#define SEG_H            Segment::vHeight()
#define SPEED_FORMULA_L  (5U + (50U*(255U - SEGMENT.speed))/SEGLEN)

// some common colors
#define RED        (uint32_t)0xFF0000
#define GREEN      (uint32_t)0x00FF00
#define BLUE       (uint32_t)0x0000FF
#define WHITE      (uint32_t)0xFFFFFF
#define BLACK      (uint32_t)0x000000
#define YELLOW     (uint32_t)0xFFFF00
#define CYAN       (uint32_t)0x00FFFF
#define MAGENTA    (uint32_t)0xFF00FF
#define PURPLE     (uint32_t)0x400080
#define ORANGE     (uint32_t)0xFF3000
#define PINK       (uint32_t)0xFF1493
#define GREY       (uint32_t)0x808080
#define GRAY       GREY
#define DARKGREY   (uint32_t)0x333333
#define DARKGRAY   DARKGREY
#define ULTRAWHITE (uint32_t)0xFFFFFFFF
#define DARKSLATEGRAY (uint32_t)0x2F4F4F
#define DARKSLATEGREY DARKSLATEGRAY
