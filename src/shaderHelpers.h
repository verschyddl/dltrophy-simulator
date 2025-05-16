//
// Created by qm210 on 13.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_SHADERHELPERS_H
#define DLTROPHY_SIMULATOR_SHADERHELPERS_H

#include <utility>
#include <variant>
#include <utility>
#include <optional>
#include <fstream>
#include <filesystem>
#include <string>
#include "FileHelper.h"

#include <glad/gl.h>
#include <glm/glm.hpp>


struct ShaderMeta {
    GLenum type;
    std::string source;
    GLuint id = 0;
    std::string error;

    explicit ShaderMeta(GLenum type): type(type) {}

    operator GLuint() const {
        return id;
    }

    void read(const std::string& path) {
        FileHelper::ensure(path);
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
    }
};

struct ProgramMeta {
    GLuint id;
    std::string error;

    operator GLuint() const {
        return id;
    }
};

template <typename T>
struct Uniform {
private:
    GLint location = 0;
    const char* name;

public:
    T value;

    explicit Uniform<T>(const std::string& name)
            : name(name.c_str())
            {}

    void loadLocation(GLuint program) {
        location = glGetUniformLocation(program, name);
    }

    void set() {
        if constexpr (std::is_same_v<T, float>) {
            glUniform1f(location, value);
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            glUniform2f(location, value.x, value.y);
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            glUniform3f(location, value.x, value.y, value.z);
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            glUniform4f(location, value.x, value.y, value.z, value.w);
        } else {
            throw std::runtime_error("Uniform.set() called for undefined type");
        }
    }

    void set(T to) {
        value = to;
        set();
    }
};

#endif //DLTROPHY_SIMULATOR_SHADERHELPERS_H
