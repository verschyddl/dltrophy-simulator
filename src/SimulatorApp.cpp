//
// Created by qm210 on 10.05.2025.
//

#include <stdexcept>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "SimulatorApp.h"

SimulatorApp::SimulatorApp(int width, int height, int port) {
    window = initializeWindow(width, height,"QM's DL Trophy Smiulator");
    receiver = new UdpReceiver(port);

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::cout << "Check OpenGL Version: " << glVersion << std::endl;

    shader = new TrophyShader(width, height);
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
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

void SimulatorApp::run() {

    glClear(GL_COLOR_BUFFER_BIT);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader->use();

        glfwSwapBuffers(window);

        handleUdpMessages();
    }

    glfwTerminate();
}

void SimulatorApp::handleUdpMessages() {
    auto package = receiver->listen();
    if (!package.has_value())
        return;

    std::cout << "GOT PACKAGE \"" << package.value().received_message
              << "\" from: " << to_string(package.value().sender)
              << std::endl;
}
