//
// Created by qm210 on 14.05.2025.
//

#include <fstream>
#include <format>
#include <nlohmann/json.hpp>
#include <iostream>
#include "Config.h"

using nlohmann::json;

Config Config::initialize(const std::string& path, GLFWwindow* window) {
    // this is meant to automatically adjust the window upon loading.
    // i.e. design feature, not reckless ignorance =P

    Config config{
        .path = std::filesystem::path(path),
    };

    if (auto rect = config.tryReadFile()) {

        glfwSetWindowPos(window, rect->x, rect->y);
        if (rect->width > 0 && rect->height > 0) {
            glfwSetWindowSize(window, rect->width, rect->height);
        }

    }

    return config;
}

std::optional<Rect> Config::tryReadFile() {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }

        json j;
        file >> j;
        Rect rect{};
        rect.x = j.value("posX", 0);
        rect.y = j.value("posY", 0);
        rect.width = j.value("width", 0);
        rect.height = j.value("height", 0);
        return std::optional(rect);

    } catch (const std::exception& e) {
        std::cerr << "Error initializing Config: " << e.what() << std::endl;
    }
    return std::nullopt;
}

void Config::store(GLFWwindow* window) const {
    auto rect = Rect::query(window);
    json j = {
            {"posX", rect.x},
            {"posY", rect.y},
            {"width", rect.width},
            {"height", rect.height}
    };

    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error(
                    std::format("Cannot open file {0}", path.string())
            );
        }
        file << j.dump(4);

    } catch (const std::exception& e) {
        std::cerr << "Error storing Config: " << e.what() << std::endl;
    }
}
