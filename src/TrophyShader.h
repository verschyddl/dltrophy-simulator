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
    ProgramMeta createProgram();
    void initializeProgram(const Config& config);
    void teardown();

    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    void initVertices();
    static std::array<float, 18> createQuadVertices();

    ShaderState *state;
    GLuint stateBufferId = 0;
    GLuint definitionBufferId = 0;
    void initUniformBuffers();

public:
    TrophyShader(const Config& config, ShaderState *state);
    ~TrophyShader();

    Uniform<float> iTime = Uniform<float>("iTime");
    Uniform<glm::vec4> iRect = Uniform<glm::vec4>("iRect");

    void use();
    void render();
    void assertCompileSuccess(const std::function<void(const std::string&)>& callback) const;
    void onRectChange(Size resolution, const Config& config);
    void reload(const Config& config);
    void mightHotReload(const Config& config);

    std::optional<std::time_t> lastReload;
    bool debugFlag = false;
};

#endif //DLTROPHY_SIMULATOR_TROPHYSHADER_H
