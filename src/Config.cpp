//
// Created by qm210 on 14.05.2025.
//

#include <fstream>
#include <format>
#include <nlohmann/json.hpp>
#include <iostream>
#include "Config.h"
#include "FileHelper.h"

using nlohmann::json;

void Config::restore(GLFWwindow* window) {
    if (!wasRead() || !windowRect.has_value()) {
        return;
    }

    glfwSetWindowPos(window, windowRect->x, windowRect->y);

    auto rect = Rect::query(window);
    if (windowRect->width > 0) {
        rect.width = windowRect->width;
    }
    if (windowRect->height > 0) {
        rect.height = windowRect->height;
    }
    glfwSetWindowSize(window, rect.width, rect.height);
}

bool Config::tryReadFile() {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        json jWindow = j["window"];
        if (jWindow.is_object()) {
            Rect rect{};
            rect.x = jWindow.value("x", 0);
            rect.y = jWindow.value("y", 0);
            rect.width = jWindow.value("width", 0);
            rect.height = jWindow.value("height", 0);
            windowRect = std::optional(rect);
        }

        json jView = j["view"];
        if (jView.is_object()) {
            shaderView.x = jView.value("x", shaderView.x);
            shaderView.y = jView.value("y", shaderView.y);
            shaderView.width = jView.value("width", shaderView.width);
            shaderView.height = jView.value("height", shaderView.height);
        }

        json jShaders = j["customShaders"];
        if (jShaders.is_object()) {
            customVertexShaderPath = jShaders.value("vertex", "");
            customFragmentShaderPath = jShaders.value("fragment", "");
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error initializing Config: " << e.what() << std::endl;
    }
    return false;
}

void Config::store(GLFWwindow* window) const {
    auto rect = Rect::query(window);
    json j = {
            {"window", {
                {"x", rect.x},
                {"y", rect.y},
                {"width", rect.width},
                {"height", rect.height}
            }},
            {"view", {
               {"x", shaderView.x},
               {"y", shaderView.y},
               {"width", shaderView.width},
               {"height", shaderView.height},
            }},
            {"customShaders", {
               {"vertex", customVertexShaderPath},
               {"fragment", customVertexShaderPath}
            }},
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

Config::Config(const std::string& filepath)
: path(std::filesystem::path(filepath)) {
    didRead = tryReadFile();
}
