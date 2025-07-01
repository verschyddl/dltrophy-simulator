//
// Created by qm210 on 20.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHY_H
#define DLTROPHY_SIMULATOR_TROPHY_H

#include <cstddef>
#include <array>
#include <functional>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <GL/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct Trophy {
    static constexpr GLuint N_LEDS_IN_BASE = 64;
    static constexpr GLuint N_LEDS_IN_LOGO = 106;
    static constexpr GLuint N_RGB_LEDS = N_LEDS_IN_BASE + N_LEDS_IN_LOGO;
    static constexpr GLuint N_SINGLE_LEDS = 2;
    static constexpr GLuint N_LEDS = N_RGB_LEDS + N_SINGLE_LEDS;

    // could be considered configurable (it's all WLED config...), but are fixed for now.
    static constexpr int baseStartIndex = 0;
    static constexpr int logoStartIndex = N_LEDS_IN_BASE;
    static constexpr int backLedIndex = N_LEDS - 2;
    static constexpr int floorLedIndex = N_LEDS - 1;

    std::array<glm::vec4, N_LEDS> position{};

    std::array<bool, N_LEDS> isSingleColor{};
    std::array<bool, N_LEDS> isLogo{};
    std::array<bool, N_LEDS> isBase{};

    glm::vec3 logoCenter = {-0.175f, 0.262f, 0.f};
    glm::vec2 logoSize = {0.5f, 0.375f};
    glm::vec3 baseCenter = {0.f, -.35f, 0.f};
    float baseSize = 1.0f;
    glm::vec3 backLedPos{-0.05f, -0.1f, 0.02f};
    glm::vec3 floorLedPos{0.0f, -.35f, 0.f};

    glm::vec3 posMin{}, posMax{};

    static const bool showSingleWhites = false; // enable when shader is more advanced

    Trophy() {
        rebuild();
    }

    void rebuild() {
        posMin = {0, 0, 0};
        posMax = {0, 0, 0};

        for (int i = 0; i < N_LEDS; i++) {

            isBase[i] = i >= baseStartIndex && i < baseStartIndex + N_LEDS_IN_BASE;
            isLogo[i] = i >= logoStartIndex && i < logoStartIndex + N_LEDS_IN_LOGO;
            isSingleColor[i] = !isLogo[i] && !isBase[i];

            glm::vec2 relative;
            glm::vec3 absolute;

            if (isLogo[i]) {
                relative = parse_logo_order(i - logoStartIndex);
                absolute = glm::vec3{
                    logoCenter.x + logoSize.x * relative.x,
                    logoCenter.y + logoSize.y * relative.y,
                    logoCenter.z
                };
            }
            else if (isBase[i]) {
                relative = calc_base_order(i - baseStartIndex);
                absolute = glm::vec3{
                    baseCenter.x + baseSize * relative.x,
                    baseCenter.y,
                    baseCenter.z + baseSize * relative.y
                };
            }
            else {
                if (!showSingleWhites) {
                    break;
                }
                if (i == floorLedIndex) {
                    absolute = floorLedPos;
                }
                else if (i == backLedIndex) {
                    absolute = backLedPos;
                } else {
                    std::cout << "[Trophy LED] Unclear what LED #" << i << " is supposed to be =/" << std::endl;
                }
            }

            position[i] = glm::vec4{
                absolute.x,
                absolute.y,
                absolute.z,
                0.f
            };

            posMin.x = std::min(posMin.x, position[i].x);
            posMin.y = std::min(posMin.y, position[i].y);
            posMin.z = std::min(posMin.z, position[i].z);
            posMax.x = std::max(posMax.x, position[i].x);
            posMax.y = std::max(posMax.y, position[i].y);
            posMax.z = std::max(posMax.z, position[i].z);
        }
    }

    static const int N_LOGO_WIDTH = 27;
    static const int N_LOGO_HEIGHT = 21;
    static const int N_LOGO_ORDER = N_LOGO_WIDTH * N_LOGO_HEIGHT;
    static const int __ = -1;
    static constexpr std::array<int, N_LOGO_ORDER> logo_order = {{
        __, __, __, 97, __, 98, __, 99, __,100, __,101, __,102, __,103, __,104, __,105, __, __, __, __, __, __, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __, 96, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, 95, __, 94, __, 93, __, 92, __, 91, __, 90, __, 89, __, 88, __, 87, __, 86, __, 85, __, __, __, __, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        73, __, 74, __, 75, __, 76, __, 77, __, 78, __, 79, __, 80, __, 81, __, 82, __, 83, __, 84, __, __, __, __,
        __, 72, __, 71, __, 70, __, 69, __, 68, __, 67, __, 66, __, 65, __, 64, __, 63, __, 62, __, 61, __, __, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __,  0, __,  1, __,  2, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, 59, __, 60, __, __,
        __, __, __,  5, __,  4, __,  3, __, __, __, __, __, __, __, __, __, __, __, __, __, 58, __, 57, __, 56, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __, __, __,  6, __,  7, __,  8, __, __, __, __, __, __, __, __, __, __, __, 53, __, 54, __, 55, __, __,
        __, __, __, __, __, 11, __, 10, __,  9, __, __, __, __, __, __, __, __, __, 52, __, 51, __, 50, __, __, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __, __, __, __, __, 12, __, 13, __, 14, __, __, __, __, __, __, __, 47, __, 48, __, 49, __, __, __, __,
        __, __, __, __, __, __, __, 17, __, 16, __, 15, __, __, __, __, __, __, __, 46, __, 45, __, __, __, __, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __, __, __, __, __, __, __, 18, __, 19, __, 20, __, 21, __, 22, __, 23, __, 24, __, 25, __, 26, __, __,
        __, __, __, __, __, __, __, __, __, 35, __, 34, __, 33, __, 32, __, 31, __, 30, __, 29, __, 28, __, 27, __,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
        __, __, __, __, __, __, __, __, __, __, 36, __, 37, __, 38, __, 39, __, 40, __, 41, __, 42, __, 43, __, 44,
    }};

    static glm::vec2 parse_logo_order(size_t logo_index) {
        auto it = std::find(logo_order.begin(), logo_order.end(), logo_index);
        auto index = (it != logo_order.end())
                ? static_cast<size_t>(std::distance(logo_order.begin(), it))
                : logo_order.size();
        auto indexX = double(index % N_LOGO_WIDTH);
        auto indexY = -double((index - indexX) / N_LOGO_WIDTH);
        glm::vec2 result{
                -0.5 + indexX / double(N_LOGO_WIDTH),
                -0.5 + indexY / double(N_LOGO_HEIGHT)
        };
        auto cos60 = cos(60. * M_PI/180.);
        auto sin60 = sin(60. * M_PI/180.);
        return {
                cos60 * result.x - sin60 * result.y - 0.5,
                sin60 * result.x + cos60 * result.y - 0.5,
        };
    }

    static glm::vec2 calc_base_order(int base_index) {
        int N_EDGE = N_LEDS_IN_BASE / 4;
        float edge_step = 1.f / float(N_EDGE - 1 + 2);
        int base_edge = base_index / N_EDGE;

        if (base_edge > 0 && base_edge < 3) {
            int y_index = (base_index % (2 * N_EDGE)) / 2;
            return glm::vec2{
                    -0.5f + (base_index % 2),
                    -0.5f + edge_step * (1 + y_index)
            };
        }
        else {
            int x_index = base_index % N_EDGE;
            return glm::vec2{
                    -0.5f + edge_step * (1 + x_index),
                    -0.5f + (base_edge > 0)
            };
        }
    }

    size_t alignedTotalSize() {
        return alignedSizeOfNumber() + alignedSizeOfPositions();
    }

    size_t alignedSizeOfNumber() {
        // first element is N_LEDS as uint -> 4 bytes -> too small.
        // from then on, the offset of an array must be a multiple of its base data size
        return sizeof(position[0]);
    }
    
    size_t alignedSizeOfPositions() {
        // position is now vec4 to match the alignment
        return position.size() * sizeof(position[0]);
    }

    void printDebug() {
        auto nLength = std::to_string(N_LEDS).length();
        std::cout << "=== DEBUG TROPHY LED POSITIONS === N = " << N_LEDS << std::endl;

        for (int i = 0; i < N_LEDS; i++) {

            if (i == logoStartIndex) {
                std::cout << "  LOGO:" << std::endl;
            } else if (i == baseStartIndex) {
                std::cout << "  BASE:" << std::endl;
            } else if (i == N_RGB_LEDS) {
                std::cout << "  WHITE-ONLY:" << std::endl;
            }

            auto p = position[i];
            // remember, the unused p.w component is only there for alignment, not used.
            std::cout << "    " << std::setw(nLength) << std::setfill('0') << i << ": "
                << p.x << ", " << p.y << ", " << p.z << std::endl;
        }

        std:: cout << "-> Ranges: " << std::endl
                   << "   X [" << posMin.x << ", " << posMax.x << "]" << std::endl
                   << "   Y [" << posMin.y << ", " << posMax.y << "]" << std::endl
                   << "   Z [" << posMin.z << ", " << posMax.z << "]" << std::endl;
        std::cout << "==================================" << std::endl;
    }

};

#endif //DLTROPHY_SIMULATOR_TROPHY_H
