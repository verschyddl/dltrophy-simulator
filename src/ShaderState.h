//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_SHADERSTATE_H
#define DLTROPHY_SIMULATOR_SHADERSTATE_H

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
    LED(GLuint R, GLuint G, GLuint B)
    : r(R), g(G), b(B) {}

    LED(GLuint W)
    : LED(W, W, W) {}

    LED() = default;

    void set(LED led) {
        r = led.r;
        g = led.g;
        b = led.b;
    }

    GLuint gray() const {
        auto w = 0.299 * r + 0.587 * g + 0.114 * b;
        return static_cast<GLuint>(w);
    }
};

struct ShaderOptions {
    bool showGrid;
    bool disableAccumulation;

    // need minimum alignment (GLint = 4 bytes), therefore:
    bool debug1 = false;
    bool debug2 = false;
};

struct Parameters {
    // if only floats (or other 4 byte-data), this makes alignment easy
    // therefore e.g. do not keep camera position as vec3 -> gets annoying
    float ledSize;
    float ledGlow;
    float camX, camY, camZ, camFov, camTilt;
    float diffuseMix, specularMix, specularGrading;
    float fogScaling, fogGrading;
    float floorSpacingX, floorSpacingZ, floorLineWidth, floorExponent, floorGrading;
    float pyramidX, pyramidY, pyramidZ, pyramidScale, pyramidHeight, pyramidAngle, pyramidAngularVelocity;
    float epoxyPermittivity;
};

struct ShaderState {
    Trophy* trophy;
    GLuint nLeds;
    std::vector<LED> leds;

    Parameters params {
        .ledSize = 0.022,
        .ledGlow = 0.8,
        .camX = 0.,
        .camY = 0.17,
        .camZ = -1.8,
        .camFov = 1.3,
        .camTilt = 12.3,
        .diffuseMix = 0.7,
        .specularMix = 0.40,
        .specularGrading = 2.1,
        .fogScaling = 0.0001,
        .fogGrading = 1.9,
        .floorSpacingX = 2.21,
        .floorSpacingZ = 5.21,
        .floorLineWidth = .05,
        .floorExponent = 30.,
        .floorGrading = 0.5,
        .pyramidX = 0.,
        .pyramidY = -.5,
        .pyramidZ = 0.,
        .pyramidScale = 1.73,
        .pyramidHeight = 0.85,
        .pyramidAngle = -10.,
        .pyramidAngularVelocity = 0.,
        .epoxyPermittivity = 1.1,
    };
    ShaderOptions options {
        .showGrid = false,
        .disableAccumulation = true,
    };

    explicit ShaderState(Trophy* trophy):
        trophy(trophy),
        nLeds(trophy->position.size())
        {
            leds.resize(nLeds);
            for (int i = 0; i < nLeds; i++) {
                leds[i] = LED();
            }
        };

    GLsizeiptr alignedTotalSize() {
        return alignedSizeForLeds()
            + sizeof(params)
            + sizeof(options);
    }

    GLsizeiptr alignedSizeForLeds() {
        return leds.size() * sizeof(leds[0]);
    }

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

#endif //DLTROPHY_SIMULATOR_SHADERSTATE_H
