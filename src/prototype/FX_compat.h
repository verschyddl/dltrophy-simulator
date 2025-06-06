//
// Created by qm210 on 06.06.2025.
//
// This is the "compability header" for the actual effects header.
// i.e. for your FX_deadlineTrophy.h, to supply what otherwise
// WLED would give you, to be interchangeable without change.
//

#ifndef DLTROPHY_SIMULATOR_FX_COMPAT_H
#define DLTROPHY_SIMULATOR_FX_COMPAT_H

#include <cstdint>

struct Segment {
    static const int maxWidth = 64;
};

struct WS2812FX {
private:
    uint8_t _segment_index;
    uint8_t _mainSegment;

public:
    inline uint8_t getCurrSegmentId(void) { return _segment_index; }

    inline void setPixelColorXY(int x, int y, uint32_t c)   { setPixelColor(y * Segment::maxWidth + x, c); }
    inline void setPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0) { setPixelColorXY(x, y, RGBW32(r,g,b,w)); }
    inline void setPixelColorXY(int x, int y, CRGB c)       { setPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0)); }

    inline uint32_t getPixelColorXY(uint16_t x, uint16_t y) {
        return getPixelColor(at2dSegment() ? y * Segment::maxWidth + x : x);
    }

};

WS2812FX strip{};

#endif //DLTROPHY_SIMULATOR_FX_COMPAT_H
