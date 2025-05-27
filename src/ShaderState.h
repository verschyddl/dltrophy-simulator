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
#include "LED.h"

struct ShaderOptions {
    // this needs minimum alignment (GLint = 4 bytes), therefore a multiple of 4 of flags.
    // (well, at the end of the std140 layout this could even deviate, but don't rely on that.)
    bool showGrid;
    bool accumulateForever;
    bool noStochasticVariation;
    bool onlyPyramidFrame;
};

struct Parameters {
    // if only floats / ints / other 4 byte-types, this makes alignment easy
    // therefore e.g. do not keep camera position as vec3 -> gets annoying
    float ledSize, ledGlow;
    float camX, camY, camZ, camFov, camTilt;
    float fogScaling, fogGrading, backgroundSpin;
    float floorSpacingX, floorSpacingZ,
          floorLineWidth, floorExponent, floorGrading;
    float pyramidX, pyramidY, pyramidZ,
          pyramidScale, pyramidHeight,
          pyramidAngle, pyramidAngularVelocity;
    float epoxyPermittivity;
    float blendPreviousMixing;
    float traceMinDistance, traceMaxDistance;
    int traceMaxSteps, traceMaxRecursions;
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
        .fogScaling = 0.0001,
        .fogGrading = 2.2,
        .backgroundSpin = 0.1,
        .floorSpacingX = 2.21,
        .floorSpacingZ = 5.21,
        .floorLineWidth = .05,
        .floorExponent = 30.,
        .floorGrading = 0.5,
        .pyramidX = 0.,
        .pyramidY = -.5,
        .pyramidZ = 0.,
        .pyramidScale = 1.26,
        .pyramidHeight = 0.85,
        .pyramidAngle = -10.,
        .pyramidAngularVelocity = 0.,
        .epoxyPermittivity = 1.1,
        .blendPreviousMixing = 0.3,
        .traceMinDistance = 1.e-3, // no reason to change
        .traceMaxDistance = 60., // 100. should be enough.
        .traceMaxSteps = 50,
        .traceMaxRecursions = 6,
    };
    ShaderOptions options {
        .showGrid = false,
        .accumulateForever = false,
        .noStochasticVariation = false,
        .onlyPyramidFrame = false,
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
