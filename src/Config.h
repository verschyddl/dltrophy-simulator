//
// Created by qm210 on 14.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_CONFIG_H
#define DLTROPHY_SIMULATOR_CONFIG_H

#include <string>
#include <filesystem>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <nlohmann/json.hpp>
#include "ShaderState.h"

struct Size {
    int width;
    int height;

    explicit operator bool() const {
        return width > 0 && height > 0;
    }
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
};

struct RelativeRect {
    float width;
    float height;
    float x;
    float y;
};

class Config {
public:
    const std::string defaultFilename = "smiuluator.cfg";
    std::filesystem::path path;

    Size windowSize {
        1080,
        720
    };
    int udpPort = 3413;

    std::string customVertexShaderPath;
    std::string customFragmentShaderPath;
    bool hotReloadShaders = true;

    Config(int argc, char* argv[]);

    void restore(GLFWwindow* window);
    void store(GLFWwindow* window, ShaderState* state = nullptr) const;

    [[nodiscard]]
    bool wasRead() const { return didRead; };

    [[nodiscard]]
    Rect shaderRect(Size resolution) const {
        auto width = static_cast<float>(resolution.width);
        auto height = static_cast<float>(resolution.height);
        return Rect{
            {
                .width = static_cast<int>(shaderView.width * width),
                .height = static_cast<int>(shaderView.height * height),
            },
            static_cast<int>(shaderView.x * width),
            static_cast<int>(shaderView.y * height),
        };
    };

    [[nodiscard]]
    inline float relativeRemainingWidth() const {
        // assumes that .x is the left margin - might change
        const float margin = shaderView.x;
        return 1.f - shaderView.width - shaderView.x - margin;
    }

private:
    bool tryReadFile();
    std::optional<nlohmann::json> tryReadJson() const;

    bool didRead = false;
    std::optional<Coord> windowPos = std::nullopt;
    RelativeRect shaderView{
            .width = 0.5,
            .height = 0.9,
            .x = 0.05,
            .y = 0.05,
    };
};

#endif //DLTROPHY_SIMULATOR_CONFIG_H
