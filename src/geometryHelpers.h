//
// Created by qm210 on 01.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H
#define DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H

#include <GLFW/glfw3.h>

struct Size {
    int width = 0;
    int height = 0;

    explicit operator bool() const {
        return width > 0 && height > 0;
    }

    int area() { return width * height; }
};

struct Coord {
    int x = 0;
    int y = 0;

    void moveToSmaller(const Coord& other) {
        if (other.x < x) {
            x = other.x;
        }
        if (other.y < y) {
            y = other.y;
        }
    }

    void moveToLarger(const Coord& other) {
        if (other.x > x) {
            x = other.x;
        }
        if (other.y > y) {
            y = other.y;
        }
    }
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

template <typename numberType1, typename numberType2>
static inline bool samePixel(numberType1 x1, numberType1 y1, numberType2 x2, numberType2 y2) {
    return std::abs(x2 - x1) < .5 && std::abs(y2 -y1) < .5;
}

static inline Coord totalMonitorMinimum() {
    int monitorCount;
    auto monitors = glfwGetMonitors(&monitorCount);
    if (monitorCount == 0) {
        throw std::runtime_error("Seems there are no monitors, what do you even want??");
    }

    Coord result{0, 0};
    for (int m = 0; m < monitorCount; m++) {
        Coord pos{};
        glfwGetMonitorPos(monitors[m], &pos.x, &pos.y);
        result.moveToSmaller(pos);
    }
    return result;
}

#endif //DLTROPHY_SIMULATOR_GEOMETRYHELPERS_H
