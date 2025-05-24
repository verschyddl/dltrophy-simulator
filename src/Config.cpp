//
// Created by qm210 on 14.05.2025.
//

#include <fstream>
#include <format>
#include <nlohmann/json.hpp>
#include <iostream>
#include <getopt.h>
#include "Config.h"
#include "FileHelper.h"

using nlohmann::json;

inline void overwrite_if_path_exists(int opt, int targetOpt, std::string& target) {
    if (opt != targetOpt) {
        return;
    }
    if (std::filesystem::exists(optarg)) {
        target = optarg;
    } else {
        std::cerr << "Ignore given path, as it is invalid: " << optarg << std::endl;
    }
}

Config::Config(int argc, char **argv) {
    std::string configFilename = defaultFilename;
    std::string overwriteVertexShaderPath = "";
    std::string overwriteFragmentShaderPath = "";

    int opt;
    while ((opt = getopt(argc, argv, "c:f:v:")) != -1) {
        if (opt == '?') {
            std::cerr << "Invalid option: " << opt << std::endl;
            continue;
        }
        overwrite_if_path_exists(opt, 'c', configFilename);
        overwrite_if_path_exists(opt, 'f', overwriteFragmentShaderPath);
        overwrite_if_path_exists(opt, 'v', overwriteVertexShaderPath);
    }

    path = std::filesystem::path(configFilename);
    didRead = tryReadFile();

    if (!overwriteVertexShaderPath.empty()) {
        customVertexShaderPath = overwriteVertexShaderPath;
    }
    if (!overwriteFragmentShaderPath.empty()) {
        customFragmentShaderPath = overwriteFragmentShaderPath;
    }
}

void Config::restore(GLFWwindow* window) {
    if (!wasRead()) {
        return;
    }

    if (windowPos.has_value()) {
        glfwSetWindowPos(window, windowPos->x, windowPos->y);
    }

    auto rect = Rect::query(window);
    if (windowSize.width > 0) {
        rect.width = windowSize.width;
    }
    if (windowSize.height > 0) {
        rect.height = windowSize.height;
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
            windowSize.width = jWindow.value(
                    "width",
                    windowSize.width
            );
            windowSize.height = jWindow.value(
                    "height",
                    windowSize.height
            );
            windowPos = std::optional(Coord{
                .x = jWindow.value("x", 0),
                .y = jWindow.value("y", 0)
            });
        }

        json jView = j["view"];
        if (jView.is_object()) {
            shaderView.x = jView.value("x", shaderView.x);
            shaderView.y = jView.value("y", shaderView.y);
            shaderView.width = jView.value("width", shaderView.width);
            shaderView.height = jView.value("height", shaderView.height);
        }

        json jShaders = j["shaders"];
        if (jShaders.is_object()) {
            customVertexShaderPath = jShaders.value("vertex", "");
            customFragmentShaderPath = jShaders.value("fragment", "");
            hotReloadShaders = jShaders.value("reload", hotReloadShaders);
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
            {"shaders", {
               {"vertex", customVertexShaderPath},
               {"fragment", customFragmentShaderPath},
               {"reload", hotReloadShaders}
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
