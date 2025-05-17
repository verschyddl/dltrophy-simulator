//
// Created by qm210 on 10.05.2025.
//

#include <format>
#include "TrophyShader.h"
#include "shaderHelpers.h"

const std::string vertex_shader_path = "./shaders/vertex.glsl";
const std::string fragment_shader_path = "./shaders/fragment.glsl";

TrophyShader::TrophyShader(Size resolution, Config config, TrophyState *state)
: state(state) {
    vertex.read(vertex_shader_path);
    fragment.read(fragment_shader_path);
    createProgram();

    glUseProgram(program);
    iTime.loadLocation(program);
    iRect.loadLocation(program);

    initStateInput();

    onRectChange(resolution, config);
}

void TrophyShader::onRectChange(Size resolution, Config config) {
    auto rect = config.shaderRect(resolution);
    iRect.value = glm::vec4(rect.x, rect.y, rect.width, rect.height);
    glViewport(rect.x, rect.y, rect.width, rect.height);
    initVertices();
}

TrophyShader::~TrophyShader() {
    if (program.id) {
        glDeleteProgram(program);
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glDeleteBuffers(1, &stateBufferId);
    glDeleteTextures(1, &stateTextureBufferId);
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

void TrophyShader::assertCompileSuccess(const std::function<void(const std::string&)>& callback) {
    std::string message;
    if (!vertex.error.empty()) {
        message += "Vertex Shader Error:\n\n" + vertex.error + "\n";
    }
    if (!fragment.error.empty()) {
        message += "Fragment Shader Error:\n\n" + fragment.error + "\n";
    }
    if (message.empty() && !program.error.empty()) {
        message += "Shader Linker Error:\n\n" + program.error + "\n";
    }
    if (!message.empty()) {
        callback(message);
    }
}

std::array<float, 18> TrophyShader::createQuadVertices() {
    const float left =  -1.f;
    const float bottom = -1.f;
    const float right = 1.f;
    const float top = 1.f;
    return {{
        left, bottom, 0.,
        right, top, 0.,
        left, top, 0.,
        left, bottom, 0.,
        right, bottom, 0.,
        right, top, 0.,
    }};
}

void TrophyShader::initVertices() {
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    auto vertices = createQuadVertices();

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(),
                 GL_STATIC_DRAW
                 );

    constexpr GLint positionAttributeLocation = 0;
    // <-- vertex.glsl must match this. (obviöslich)
    //     could also use glGetAttribLocation(),
    //     but then names must match, so... little gained.

    glVertexAttribPointer(
            positionAttributeLocation,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            (void*)0
            );
    glEnableVertexAttribArray(positionAttributeLocation);
}

void TrophyShader::initStateInput() {
    // wir wollen UNIFORM BUFFER, nicht BUFFER TEXTURE OBJECT.
    // ... glaub ich.
    glGenBuffers(1, &stateBufferId);
    glBindBuffer(GL_UNIFORM_BUFFER, stateBufferId);
    glBufferData(GL_UNIFORM_BUFFER,
                 sizeof(state->leds),
                 nullptr,
                 GL_STATIC_DRAW
                 );
    GLuint bindingPoint = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, stateBufferId);

    // glBindBuffer(GL_UNIFORM_BUFFER, 0); // <-- unbind again

//    glGenTextures(1, &stateTextureBufferId);
//    glBindTexture(GL_TEXTURE_BUFFER, stateTextureBufferId);
//    glTexBuffer(GL_TEXTURE_BUFFER,
//                GL_RGB32UI,
//                stateBufferId
//                );
}

void TrophyShader::use(float time) {
    if (!program.error.empty()) {
        throw std::runtime_error("Cannot use program, because linking failed.");
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.id);

    iTime.set(time);
    iRect.set();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, stateTextureBufferId);

}

void TrophyShader::draw() {
    auto ledData = state->leds.data();
    glBindBuffer(GL_UNIFORM_BUFFER, stateBufferId);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    0,
                    sizeof(state->leds),
                    state->leds.data()
                    );

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, stateBufferId);

    // guess this re-binding is unnecessary... anyway.
    glBindVertexArray(vertexArrayObject);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
