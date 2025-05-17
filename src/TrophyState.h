//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSTATE_H
#define DLTROPHY_SIMULATOR_TROPHYSTATE_H

#include <cstdint>
#include <array>
#include <span>

struct LED {

    LED(uint8_t R, uint8_t G, uint8_t B) {
        set(R, G, B);
    }

    LED(uint8_t W) {
        set(W);
    }

    LED() = default;

    void set(uint8_t R, uint8_t G, uint8_t B) {
        r = R;
        g = G;
        b = B;
    }

    void set(uint8_t w) {
        r = w;
        g = w;
        b = w;
    }

    void set(LED led) {
        set(led.r, led.g, led.b);
    }

    uint8_t gray() {
        float w = 0.299 * r + 0.587 * g + 0.114 * b;
        return static_cast<uint8_t>(w);
    }

private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct TrophyState {
    static const int N_LEDS_IN_LOGO = 106;
    static const int N_LEDS_IN_BASE = 64;
    static const int N_RGB_LEDS = N_LEDS_IN_LOGO + N_LEDS_IN_BASE;
    static const int N_SINGLE_LEDS = 2;
    static const int N_LEDS = N_RGB_LEDS + N_SINGLE_LEDS;

    std::array<LED, N_LEDS> leds;
    std::array<bool, N_LEDS> is_single_color;

    TrophyState() {
        for (int i = 0; i < N_LEDS; i++) {
            leds[i] = LED();
            is_single_color[i] = i > N_RGB_LEDS;
        }
    };

    void set(size_t index, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) {
        if (index >= N_LEDS) {
            throw std::runtime_error(
                    std::format("Trophy has no LED at index {0}", index)
            );
        }
        if (is_single_color[index]) {
            leds[index].set(r);
        } else {
            leds[index].set(r, g, b);
        }
    }

    size_t alignedSize() {
        // to put 3 uint8_t into alignments of 4
        // shader might do some unfug otherwise
        return leds.size() * 4;
    }

    std::span<LED> logo() {
        return std::span<LED>(
                leds.data(),
                N_LEDS_IN_LOGO
                );
    }

    std::span<LED> base() {
        return std::span<LED>(
                leds.data() + N_LEDS_IN_LOGO + 1,
                N_LEDS_IN_BASE
                );
    }

    LED floor() {
        return leds[N_RGB_LEDS + 1];
    }

    LED back() {
        return leds[N_RGB_LEDS + 2];
    }

    std::span<LED> singleWhites() {
        return std::span<LED>(
                leds.data() + N_LEDS - N_SINGLE_LEDS + 1,
                N_SINGLE_LEDS
                );
    }

    void randomizeForDebugging() {
        for (int i = 0; i < N_LEDS; i++) {
            uint8_t r = std::rand() % 256;
            uint8_t g = std::rand() % 256;
            uint8_t b = std::rand() % 256;
            set(i, r, g, b);
        }
    }
};

#endif //DLTROPHY_SIMULATOR_TROPHYSTATE_H
