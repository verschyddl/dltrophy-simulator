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
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glad/gl.h"
#include "glm/vec4.hpp"

struct Trophy {

    static const GLuint N_LEDS_IN_LOGO = 106;
    static const GLuint N_LEDS_IN_BASE = 64;
    static const GLuint N_RGB_LEDS = N_LEDS_IN_LOGO + N_LEDS_IN_BASE;
    static const GLuint N_SINGLE_LEDS = 2;
    static const GLuint N_LEDS = N_RGB_LEDS + N_SINGLE_LEDS;

    std::array<glm::vec4, N_LEDS> position;

    std::array<bool, N_LEDS> is_single_color;
    std::array<bool, N_LEDS> is_logo;
    std::array<bool, N_LEDS> is_base;

    static constexpr glm::vec3 logo_center = {0.f, 0.7f, 0.f};
    static constexpr float logo_width = 1.2f;
    static constexpr float logo_height = 0.5f;
    static constexpr glm::vec3 base_center = {0.f, -.5f, 0.f};
    static constexpr float base_size = 0.3f;

    glm::vec3 pos_min, pos_max;

    Trophy() {
        for (int i = 0; i < N_LEDS; i++) {
            is_single_color[i] = i >= N_RGB_LEDS;
            is_logo[i] = i < N_LEDS_IN_LOGO;
            is_base[i] = !is_logo[i] && i < N_RGB_LEDS;

            glm::vec2 relative;
            glm::vec3 absolute;

            if (is_logo[i]) {
                relative = parse_logo_order(i);
                absolute = glm::vec3(
                        logo_center.x + logo_width * relative.x,
                        logo_center.y + logo_height * relative.y,
                        logo_center.z
                );
            }
            else if (is_base[i]) {
                relative = calc_base_order(i - N_LEDS_IN_LOGO);
                absolute = glm::vec3(
                        base_center.x + base_size * relative.x,
                        base_center.y,
                        base_center.z + base_size * relative.y
                );
            }
            else {
                absolute = glm::vec3(
                        0.0f,
                        0.0f,
                        -2.1f // irgendwoanders hin halt
                );
            }

            position[i] = glm::vec4{
                absolute.x,
                absolute.y,
                absolute.z,
                0.f
            };

            pos_min.x = std::min(pos_min.x, position[i].x);
            pos_min.y = std::min(pos_min.y, position[i].y);
            pos_min.z = std::min(pos_min.z, position[i].z);
            pos_max.x = std::max(pos_max.x, position[i].x);
            pos_max.y = std::max(pos_max.y, position[i].y);
            pos_max.z = std::max(pos_max.z, position[i].z);
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
        auto indexY = double((index - indexX) / N_LOGO_WIDTH);
        return glm::vec2{
            -0.5 + indexX / double(N_LOGO_WIDTH),
            -0.5 + indexY / double(N_LOGO_HEIGHT)
        };
    }

    static glm::vec2 calc_base_order(size_t base_index) {
        size_t N_EDGE = N_LEDS_IN_BASE / 4;
        float edge_step = 1. / float(N_EDGE + 2);

        if (base_index / N_EDGE > 0 && base_index / N_EDGE < 3) {
            size_t y_index = base_index % (2 * N_EDGE);
            return glm::vec2{
                    -0.5f + (base_index % 2),
                    -0.5f + edge_step * (1 + y_index)
            };
        }
        else {
            size_t x_index = base_index % N_EDGE;
            return glm::vec2{
                    -0.5f + edge_step * (1 + x_index),
                    -0.5f + (base_index / N_EDGE > 0)
            };
        }
    }

    GLsizeiptr alignedTotalSize() {
        return alignedSizeOfNumber() + alignedSizeOfPositions();
    }

    GLsizeiptr alignedSizeOfNumber() {
        // first element is N_LEDS as uint -> 4 bytes -> too small.
        // from then on, the offset of an array must be a multiple of its base data size
        return sizeof(position[0]);
    }
    
    GLsizeiptr alignedSizeOfPositions() {
        // position is now vec4 to match the alignment
        return position.size() * sizeof(position[0]);
    }

    void printDebug() {
        auto nLength = std::to_string(N_LEDS).length();
        std::cout << "=== DEBUG TROPHY LED POSITIONS === N = " << N_LEDS << std::endl;

        for (int i = 0; i < N_LEDS; i++) {

            if (i == 0) {
                std::cout << "  LOGO:" << std::endl;
            } else if (i == N_LEDS_IN_LOGO) {
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
                   << "   X [" << pos_min.x << ", " << pos_max.x << "]" << std::endl
                   << "   Y [" << pos_min.y << ", " << pos_max.y << "]" << std::endl
                   << "   Z [" << pos_min.z << ", " << pos_max.z << "]" << std::endl;
        std::cout << "==================================" << std::endl;
    }

};

#endif //DLTROPHY_SIMULATOR_TROPHY_H
