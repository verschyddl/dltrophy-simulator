//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_TROPHYSHADER_H
#define DLTROPHY_SIMULATOR_TROPHYSHADER_H

#include <string>
#include <utility>
#include <vector>
#include <functional>

#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "shaderHelpers.h"
#include "Config.h"
#include "ShaderState.h"

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

    ShaderState *state;
    GLuint stateBufferId = 0;
    GLuint definitionBufferId = 0;
    void initUniformBuffers();

    // TODO: think about unified handling of uniforms... somehow... someday...
    Uniform<float> iTime = Uniform<float>("iTime");
    Uniform<glm::vec4> iRect = Uniform<glm::vec4>("iRect");

public:
    TrophyShader(Size resolution, Config config, ShaderState *state);
    ~TrophyShader();

    void use();
    void draw(float time);
    void assertCompileSuccess(const std::function<void(const std::string&)>& callback);
    void onRectChange(Size resolution, Config config);
};

#endif //DLTROPHY_SIMULATOR_TROPHYSHADER_H
