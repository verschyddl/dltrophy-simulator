//
// Created by qm210 on 10.05.2025.
//

#include <stdexcept>
#include <iostream>

#include "SimulatorApp.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

SimulatorApp::SimulatorApp(int width, int height, int port) {
    window = initializeWindow(width, height,"QM's DL Trophy Smiulator");

    gladLoadGL(glfwGetProcAddress);

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::cout << "Check OpenGL Version: " << glVersion << std::endl;

    if ((void*)glCreateShader == nullptr) {
        throw std::runtime_error("OpenGL not actually loaded - bad.");
    };

    shader = new TrophyShader(width, height);

    receiver = new UdpReceiver(port);
}

SimulatorApp::~SimulatorApp() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

GLFWwindow* SimulatorApp::initializeWindow(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        throw std::runtime_error("Cannot initialize GLFW");
    }

    glfwSetErrorCallback(handleWindowError);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Could not create GLFW window.");
    }

    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    return window;
}

void SimulatorApp::handleWindowError(int error, const char* description) {
    std::cerr << "Error: " << description << " (" << error << ")" << std::endl;
}

void SimulatorApp::run() {
    // Create vertex array object and vertex buffer object
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    // Setup triangle vertices
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    // Fill vertex buffer
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Fragment Shader Example");
        ImGui::Text("This is a window with a fragment shader!");
        ImGui::End();

        shader->use();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        handleUdpMessages();
        handleInput();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void SimulatorApp::handleInput() {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        std::cout << "Mouse Down Left @ (" << mouseX << ", " << mouseY << ")" << std::endl;
    }
}

void SimulatorApp::handleUdpMessages() {
    auto package = receiver->listen();
    if (!package.has_value())
        return;

    std::cout << "GOT PACKAGE \"" << package.value().received_message
              << "\" from: " << to_string(package.value().sender)
              << std::endl;
}
