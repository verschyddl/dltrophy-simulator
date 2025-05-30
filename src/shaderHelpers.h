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
#include <map>
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
        error = "";
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
    GLint location = -1;
    std::string name;
    bool triedLoadLocation = false;
    bool hasLocationWarned = false;

public:
    T value;

    explicit Uniform<T>(std::string  name)
            : name(std::move(name))
            {}

    void loadLocation(GLuint program) {
        location = glGetUniformLocation(program, name.c_str());
        triedLoadLocation = true;
    }

    void set() {
        if (location < 0) {
            if (!triedLoadLocation && !hasLocationWarned) {
                std::cerr << "Uniform was never initialized: " << name << std::endl;
                hasLocationWarned = true;
            }
            return;
        }
        if constexpr (std::is_same_v<T, float>) {
            glUniform1f(location, value);
        } else if constexpr (std::is_same_v<T, int>) {
            glUniform1i(location, value);
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

    void printDebug() const {
        std::string printValue;
        if constexpr (std::is_same_v<T, glm::vec4>) {
            printValue = std::format("vec4({}, {}, {}, {})", value.x, value.y, value.z, value.w);
        } else {
            // add other cases when they actually occur
            printValue = std::format("{}", value);
        }
        std::cout << "[Debug Uniform] "
                  << std::format("uniform (location = {}) {} = {};", location, name, printValue)
                  << std::endl;
    }
};

struct Framebuffer {
    GLuint object;
    GLuint texture;
    GLenum status;
    std::string label;

    explicit Framebuffer(const std::string& label): label(std::move(label)) {}

    void assertStatus() const {
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error(std::format(
                    "Error in Framebuffer \"{}\": {} ({})",
                    label, StatusMessages.at(status), status
            ));
        }
    }

    static inline const std::map<GLenum, std::string> StatusMessages = {
            {GL_FRAMEBUFFER_COMPLETE, "Framebuffer complete"},
            {GL_FRAMEBUFFER_UNDEFINED, "Framebuffer undefined"},
            {GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, "Framebuffer incomplete attachment"},
            {GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, "Framebuffer incomplete missing attachment"},
            {GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, "Framebuffer incomplete draw buffer"},
            {GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, "Framebuffer incomplete read buffer"},
            {GL_FRAMEBUFFER_UNSUPPORTED, "Framebuffer unsupported"},
            {GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, "Framebuffer incomplete multisample"},
            {GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS, "Framebuffer incomplete layer targets"}
    };

};

struct FramebufferPingPong {
    static const int N = 2;
    std::array<GLuint, N> object{};
    std::array<GLuint, N> texture{};
    std::array<GLenum, N> status{};
    int pingCursor = 0;

    FramebufferPingPong() = default;

    std::pair<GLuint, GLuint> getOrderAndAdvance() {
        auto pongCursor = (pingCursor + 1) % N;
        std::pair<GLuint, GLuint> result{pingCursor, pongCursor};
        pingCursor = pongCursor;
        return result;
    }

    void assertStatus(int i) const {
        if (status[i] != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error(std::format(
                    "Error in Ping Pong Framebuffer {}: {} ({})",
                    i, Framebuffer::StatusMessages.at(status[i]), status[i]
            ));
        }
    }
};

#endif //DLTROPHY_SIMULATOR_SHADERHELPERS_H
