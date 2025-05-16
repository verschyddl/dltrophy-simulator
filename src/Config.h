//
// Created by qm210 on 14.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_CONFIG_H
#define DLTROPHY_SIMULATOR_CONFIG_H

#include <string>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <filesystem>

struct Rect {
    int width;
    int height;
    int x;
    int y;

//    static std::unique_ptr<Rect> query(GLFWwindow* window) {
//        auto result = std::make_unique<Rect>();
    static Rect query(GLFWwindow* window) {
        Rect result{};
        glfwGetWindowSize(window, &result.width, &result.height);
        glfwGetWindowPos(window, &result.x, &result.y);
        return result;
    }
};

struct Config {
public:
    std::filesystem::path path;

    void store(GLFWwindow* window) const;
    static Config initialize(const std::string& path, GLFWwindow* window);

private:
    std::optional<Rect> tryReadFile();
};


#endif //DLTROPHY_SIMULATOR_CONFIG_H
