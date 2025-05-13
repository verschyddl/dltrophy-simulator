//
// Created by qm210 on 10.05.2025.
//

#include <format>
#include "TrophyShader.h"

const std::string vertex_shader_path = "./shaders/vertex.glsl";
const std::string fragment_shader_path = "./shaders/fragment.glsl";

TrophyShader::TrophyShader(int width, int height) {
    ensure_path(vertex_shader_path);
    vertex.read(vertex_shader_path);
    ensure_path(fragment_shader_path);
    fragment.read(fragment_shader_path);
    createProgram();

    glViewport(0, 0, width, height);
}

void TrophyShader::ensure_path(const std::string& path) {
    // helper that could be moved out of this class, but nevermindelidoo.
    if (std::filesystem::exists(path))
        return;

    auto absolute_path =
            std::filesystem::absolute(
                    std::filesystem::path(path)
            ).string();
    auto message = std::format(
            "File can not be read under {0}",
            absolute_path
            );
    throw std::runtime_error(message);
}

TrophyShader::~TrophyShader() {
    if (program.id) {
        glDeleteProgram(program);
    }
}

void compileShader(ShaderMeta& shader) {
    shader.id = glCreateShader(shader.type);
    const char* source = shader.source.c_str();
    glShaderSource(shader.id, 1, &source, nullptr);
    glCompileShader(shader.id);

    GLint status;
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &length);
        shader.error.assign(length, ' ');
        glGetShaderInfoLog(shader.id, length, nullptr, &shader.error[0]);
        glDeleteShader(shader.id);
    }
}

void TrophyShader::createProgram() {
    auto v = glCreateShader(GL_VERTEX_SHADER);
    auto f = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(vertex);
    compileShader(fragment);

    program.id = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, &program.error[0]);
        glDeleteProgram(program);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void TrophyShader::use() {
    if (!program.error.empty()) {
        throw std::runtime_error("Cannot use program, because linking failed.");
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.id);
}
