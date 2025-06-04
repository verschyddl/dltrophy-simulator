//
// Created by qm210 on 01.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H
#define DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H

#include "GLFW/glfw3.h"

struct Size {
    int width;
    int height;

    explicit operator bool() const {
        return width > 0 && height > 0;
    }

    int area() { return width * height; }
};

struct Coord {
    int x;
    int y;
};

struct Rect : public Size, public Coord {
    static Rect query(GLFWwindow* window) {
        Rect result{};
        glfwGetWindowSize(window, &result.width, &result.height);
        glfwGetWindowPos(window, &result.x, &result.y);
        return result;
    }

    int maxX() const { return x + width; }
    int maxY() const { return y + height; }
    Size extent() const { return {maxX(), maxY()}; }

};

struct RelativeRect {
    float width;
    float height;
    float x;
    float y;
};

#endif //DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H
