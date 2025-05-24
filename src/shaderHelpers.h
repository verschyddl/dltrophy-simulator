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
    std::string filePath;
    std::filesystem::file_time_type fileTime;

    explicit ShaderMeta(GLenum type): type(type) {}

    operator GLuint() const {
        return id;
    }

    void read(const std::string& path) {
        FileHelper::ensure(path);
        filePath = path;
        read();
    }

    void read() {
        std::ifstream file(filePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();

        fileTime = std::filesystem::last_write_time(filePath);
    }

    void compile() {
        id = glCreateShader(type);
        const char* source_str = source.c_str();
        glShaderSource(id, 1, &source_str, nullptr);
        glCompileShader(id);
        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            error.assign(length, ' ');
            glGetShaderInfoLog(id, length, nullptr, &error[0]);
            glDeleteShader(id);
        }
    }

    [[nodiscard]]
    bool fileHasChanged() const {
        auto time = std::filesystem::last_write_time(filePath);
        return fileTime != time;
    }
};

struct ProgramMeta {
    GLuint id;
    std::string error;

    operator GLuint() const {
        return id;
    }

    [[nodiscard]]
    bool works() const {
        return id > 0 && error.empty();
    }
};

template <typename T>
struct Uniform {
private:
    GLint location = 0;
    std::string name;

public:
    T value;

    explicit Uniform<T>(const std::string& name)
            : name(name)
            {}

    void loadLocation(GLuint program) {
        location = glGetUniformLocation(program, name.c_str());
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
