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
#include "Trophy.h"

struct LED {

private:
    GLuint r;
    GLuint g;
    GLuint b;
    // this struct must match a multiple of 4 bytes
    // to be passed directly into the buffers, thus:
    [[maybe_unused]] GLuint _unusedAlignment = 0;

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
    Trophy* trophy;
    GLuint nLeds;
    std::vector<LED> leds;

    explicit TrophyState(Trophy* trophy):
        trophy(trophy),
        nLeds(trophy->N_LEDS)
        {
            leds.resize(nLeds);
            for (int i = 0; i < nLeds; i++) {
                leds[i] = LED();
            }
        };

    void set(size_t index, LED led) {
        if (index >= nLeds) {
            throw std::runtime_error(
                    std::format("Trophy has no LED at index {0}", index)
            );
        }
        if (trophy->is_single_color[index]) {
            leds[index].set(led.gray());
        } else {
            leds[index].set(led);
        }
    }

    void set(size_t index, GLuint r = 0, GLuint g = 0, GLuint b = 0) {
        set(index, LED(r, g, b));
    }

    // The following functions are here for debugging / development

    void set(std::function<LED(size_t)> func) {
        for (int i = 0; i < nLeds; i++) {
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

    void randomize() {
        for (int i = 0; i < nLeds; i++) {
            uint8_t r = std::rand() % 256;
            uint8_t g = std::rand() % 256;
            uint8_t b = std::rand() % 256;
            set(i, r, g, b);
        }
    }
};

#endif //DLTROPHY_SIMULATOR_TROPHYSTATE_H
