//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSHADER_H
#define DLTROPHY_SIMULATOR_TROPHYSHADER_H

#include <string>
#include <utility>
#include <vector>

#include <glad/gl.h>
#include "../cmake-build-debug/_deps/glm-src/glm/vec2.hpp"

#include "shaderHelpers.h"

class TrophyShader {

private:
    ShaderMeta vertex = ShaderMeta(GL_VERTEX_SHADER);
    ShaderMeta fragment = ShaderMeta(GL_FRAGMENT_SHADER);
    ProgramMeta program;
    void createProgram();

    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    void initVertices();
    static std::array<float, 18> createQuadVertices();

    // TODO: think about unified handling of uniforms... somehow... someday...
    Uniform<float> iTime = Uniform<float>("iTime");
    Uniform<glm::vec4> iRect = Uniform<glm::vec4>("iRect");

public:
    TrophyShader(int width, int height);
    ~TrophyShader();

    void use(float time);
    void draw();

    void onRectChange();
};

#endif //DLTROPHY_SIMULATOR_TROPHYSHADER_H
