//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSTATE_H
#define DLTROPHY_SIMULATOR_TROPHYSTATE_H

#include <cstdint>

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    RGB(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b) {}

    RGB() = default;
};

struct TrophyState {
    static const int LOGO_LEDS = 106;
    static const int BASE_LEDS = 64;

    RGB logo[LOGO_LEDS];
    RGB base[BASE_LEDS];
    uint8_t floor;
    uint8_t back;

    TrophyState() = default;
};

#endif //DLTROPHY_SIMULATOR_TROPHYSTATE_H
