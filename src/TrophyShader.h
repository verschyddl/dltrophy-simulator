//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSHADER_H
#define DLTROPHY_SIMULATOR_TROPHYSHADER_H

#include <string>
#include <filesystem>
#include <fstream>
#include <optional>
#include <utility>
#include <glad/gl.h>

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
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
        file.close();
    }
};

struct ProgramMeta {
    GLuint id;
    std::string error;

    operator GLuint() const {
        return id;
    }
};

class TrophyShader {

private:
    ShaderMeta vertex = ShaderMeta(GL_VERTEX_SHADER);
    ShaderMeta fragment = ShaderMeta(GL_FRAGMENT_SHADER);
    ProgramMeta program;
    void createProgram();

public:
    TrophyShader(int width, int height);
    ~TrophyShader();

    void use();
    void update();
    void draw();

    static void ensure_path(const std::string&);
};

#endif //DLTROPHY_SIMULATOR_TROPHYSHADER_H
