//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSTATE_H
#define DLTROPHY_SIMULATOR_TROPHYSTATE_H

#include <cstdint>
#include <array>
#include <span>
#include <functional>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

struct LED {

private:
    GLuint r;
    GLuint g;
    GLuint b;
    // this struct must match a multiple of 4 bytes
    // to be passed directly into the buffers, thus:
    GLuint _unusedAlignment = 0;

public:
    LED(GLuint R, GLuint G, GLuint B) {
        set(R, G, B);
    }

    LED(GLuint W) {
        set(W);
    }

    LED() = default;

    void set(GLuint R, GLuint G, GLuint B) {
        r = R;
        g = G;
        b = B;
    }

    void set(GLuint w) {
        r = w;
        g = w;
        b = w;
    }

    void set(LED led) {
        set(led.r, led.g, led.b);
    }

    GLuint gray() {
        float w = 0.299 * r + 0.587 * g + 0.114 * b;
        return static_cast<GLuint>(w);
    }
};

struct TrophyState {
    static const size_t N_LEDS_IN_LOGO = 106;
    static const size_t N_LEDS_IN_BASE = 64;
    static const size_t N_RGB_LEDS = N_LEDS_IN_LOGO + N_LEDS_IN_BASE;
    static const size_t N_SINGLE_LEDS = 2;
    static const size_t N_LEDS = N_RGB_LEDS + N_SINGLE_LEDS;

    std::array<LED, N_LEDS> leds;
    std::array<bool, N_LEDS> is_single_color;
    std::array<glm::vec3, N_LEDS> position;

    TrophyState() {
        for (int i = 0; i < N_LEDS; i++) {
            leds[i] = LED();
            is_single_color[i] = i >= N_RGB_LEDS;

            // TODO: define position vector correctly
            position[i] = glm::vec3(
                    0.0f,
                    0.0f,
                    0.0f
            );
        }

    };

    void set(size_t index, LED led) {
        if (index >= N_LEDS) {
            throw std::runtime_error(
                    std::format("Trophy has no LED at index {0}", index)
            );
        }
        if (is_single_color[index]) {
            leds[index].set(led.gray());
        } else {
            leds[index].set(led);
        }
    }

    void set(size_t index, GLuint r = 0, GLuint g = 0, GLuint b = 0) {
        set(index, LED(r, g, b));
    }

    void set(std::function<LED(size_t)> func) {
        for (int i = 0; i < N_LEDS; i++) {
            auto led = func(i);
            set(i, led);
        }
    }

    void setAll(uint8_t r, uint8_t g, uint8_t b) {
        auto led = LED(r, g, b);
        set([&led](size_t index) {
            return led;
        });
    }

    size_t totalDefinitionSize() {
        // first element is GLuint N_LEDS -> 4 bytes
        // position is vec3 array -> 12 bytes -> needs 16 bytes each
        return sizeof(N_LEDS) + N_LEDS * 16;
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
