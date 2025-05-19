//
// Created by qm210 on 14.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_CONFIG_H
#define DLTROPHY_SIMULATOR_CONFIG_H

#include <string>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <filesystem>

struct Size {
    int width;
    int height;

    explicit operator bool() const {
        return width > 0 && height > 0;
    }
};

struct Rect : public Size {
    int x;
    int y;

    static Rect query(GLFWwindow* window) {
        Rect result{};
        glfwGetWindowSize(window, &result.width, &result.height);
        glfwGetWindowPos(window, &result.x, &result.y);
        return result;
    }
};

struct RelativeRect {
    float width;
    float height;
    float x;
    float y;
};

class Config {
public:
    std::filesystem::path path;

    explicit Config(const std::string& filepath);

    void restore(GLFWwindow* window);
    void store(GLFWwindow* window) const;

    bool wasRead() { return didRead; };

    Rect shaderRect(Size resolution) {
        return Rect{
            {
                .width = static_cast<int>(shaderView.width * resolution.width),
                .height = static_cast<int>(shaderView.height * resolution.height),
            },
            static_cast<int>(shaderView.x * resolution.width),
            static_cast<int>(shaderView.y * resolution.height),
        };
    };

private:
    bool didRead = false;
    std::optional<Rect> windowRect = std::nullopt;
    RelativeRect shaderView{
            .width = 0.5,
            .height = 0.9,
            .x = 0.47,
            .y = 0.05,
    };

    bool tryReadFile();

};

#endif //DLTROPHY_SIMULATOR_CONFIG_H
