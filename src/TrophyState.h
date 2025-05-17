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

    LED(bool has_rgb): is_single_pwm(!has_rgb) {}

    void set(uint8_t R, uint8_t G, uint8_t B) {
        if (is_single_pwm) {
            set(gray());
            return;
        }
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

    // TODO: care about the Single-PWM LEDS later
    // as I do not know whether they get 3 values also
    // -- the UDP protocol does not know LED type...
    bool is_single_pwm = false;

};

struct TrophyState {
    static const int N_LEDS_IN_LOGO = 106;
    static const int N_LEDS_IN_BASE = 64;
    static const int N_RGB_LEDS = N_LEDS_IN_LOGO + N_LEDS_IN_BASE;
    static const int N_SINGLE_LEDS = 2;
    static const int N_LEDS = N_RGB_LEDS + N_SINGLE_LEDS;

    std::array<LED, N_LEDS> leds;

    TrophyState() {
        for (int i = 0; i < N_RGB_LEDS; i++) {
            leds[i] = LED();
        }
        for (int i = 0; i < N_SINGLE_LEDS; i++) {
            leds[N_RGB_LEDS + i] = LED(false);
        }
    };

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
            leds[i].set(r, g, b);
        }
    }
};

#endif //DLTROPHY_SIMULATOR_TROPHYSTATE_H
