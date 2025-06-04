//
// Created by qm210 on 01.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_DEBUG_H
#define DLTROPHY_SIMULATOR_DEBUG_H

#include <iostream>
#include <string>
#include "geometryHelpers.h"
#include "glm/vec4.hpp"

static inline void print(const std::string& label, Rect rect) {
    std::cout << label << ": "
              << rect.x << ", " << rect.y
              << ", size: " << rect.width << ", " << rect.height
              << std::endl;
}

static inline void print(const std::string& label, glm::vec4 vec4) {
    std::cout << label << ": ("
              << vec4.x << ", " << vec4.y << ", " << vec4.z << ", " << vec4.w
              << ")" << std::endl;
}

#endif //DLTROPHY_SIMULATOR_DEBUG_H
