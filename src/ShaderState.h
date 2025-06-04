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
#include "geometryHelpers.h"
#include "debug.h"

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
    float floorLevel, floorSpacingX, floorSpacingZ,
          floorLineWidth, floorExponent, floorGrading;
    float pyramidX, pyramidY, pyramidZ,
          pyramidScale, pyramidHeight,
          pyramidAngle, pyramidAngularVelocity;
    float epoxyPermittivity;
    float blendPreviousMixing;
    float traceMinDistance, traceMaxDistance, traceFixedStep;
    int traceMaxSteps, traceMaxRecursions;
};

struct ShaderState {
    Trophy* trophy;
    GLuint nLeds;
    std::vector<LED> leds;

    Parameters params {
        .ledSize = 0.015,
        .ledGlow = 0.8,
        .camX = 0.,
        .camY = 0.17,
        .camZ = -1.8,
        .camFov = 1.3,
        .camTilt = 12.3,
        .fogScaling = 0.0001,
        .fogGrading = 2.2,
        .backgroundSpin = 0.1,
        .floorLevel = -4.,
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
        .epoxyPermittivity = 1.6,
        .blendPreviousMixing = 0.35,
        .traceMinDistance = 1.e-3, // no reason to change
        .traceMaxDistance = 60., // 100. should be enough.
        .traceFixedStep = 0.1,
        .traceMaxSteps = 40,
        .traceMaxRecursions = 6,
    };
    ShaderOptions options {
        .showGrid = false,
        .accumulateForever = false,
        .noStochasticVariation = false,
        .onlyPyramidFrame = true,
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

    GLsizei alignedTotalSize() {
        return alignedSizeForLeds()
                + sizeof(params)
                + sizeof(options);
    }

    GLsizei alignedSizeForLeds() {
        return static_cast<GLsizei>(leds.size()) * sizeof(leds[0]);
    }

    void set(size_t index, LED led, bool silent = false) {
        if (index >= nLeds) {
            if (silent) {
                return;
            }
            throw std::runtime_error(
                    std::format("Trophy has no LED at index {0}", index)
            );
        }
        if (trophy->isSingleColor[index]) {
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

struct ExtraOutputs {
    // we allow ourselves to get one extra vec4 (i.e. four extra floats)
    // from the rendering. these are then distinguished in here.

private:
    Rect rect_;
    std::vector<float> values;
    std::vector<float> testOutput_;
    std::optional<int> clickedLedIndex_;
    std::vector<float> unusedZ_;
    std::vector<float> unusedW_;

    static constexpr float noLedClicked = -1.f;
    static constexpr float uninitialized = -0.123f;

public:
    float* data() { return values.data(); }
    const Rect& rect() { return rect_; }
    const int width() { return rect_.maxX(); }
    const int height() { return rect_.maxY(); }
    const std::vector<float> testOutput() { return testOutput_; }
    const std::optional<int> clickedLedIndex() { return clickedLedIndex_; }

    void initialize(Rect rect) {
        rect_ = rect;
        auto totalSize = rect.extent().area();
        values = std::vector<float>(4 * totalSize, uninitialized);
        testOutput_.resize(totalSize);
        clickedLedIndex_ = std::nullopt;
    }

    void interpretValues(void* data, glm::vec4 iMouse, float time) {
        float* values = static_cast<float*>(data);
        glm::vec4* vecValues = static_cast<glm::vec4*>(data);
//        auto size = values.size();
//        values = std::vector<float>(floatData, floatData + size);

        clickedLedIndex_ = std::nullopt;
        int index = 0;
        for (int iy = 0; iy < height(); iy++)
            for (int ix = 0; ix < width(); ix++) {

                auto testVec4 = vecValues[index];

                auto valueX = values[4*index];
                auto valueY = values[4*index+1];
                auto valueZ = values[4*index+2];
                auto valueW = values[4*index+3];

                if (abs(ix - iMouse.z) < 1 && abs(iy - iMouse.w) < 1) {
                    std::cout << "Values @ Cursor: "
                              << valueX << ", "
                              << valueY << ", "
                              << valueZ << ", "
                              << valueW
                              << " -- " << ix << " ~ " << iMouse.z << "; " << iy << " ~ " << iMouse.w
                              << std::endl;
                }

                testOutput_[index] = valueX;
                if (testOutput_[index] == 0.f) {
                    // this means the main() has not be called on this value
                    continue;
                }

                if (valueY != noLedClicked) {
                    clickedLedIndex_ = static_cast<int>(valueY);
                }

                std::cout << "    [HARDCORE DEBUG] "
                          << std::setw(3) << ix << " "
                          << std::setw(3) << iy << " ("
                          << valueX << ", "
                          << valueY << ", "
                          << valueZ << ", "
                          << valueW << ") "
                          << std::endl;

                index++;
        }

        std::cout << "  Time: " << time << " -- ";
        print("Rect", rect_);
    }
};

#endif //DLTROPHY_SIMULATOR_SHADERSTATE_H
