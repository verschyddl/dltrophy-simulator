//
// Created by qm210 on 10.05.2025.
//

#include <stdexcept>
#include <iostream>

#include "SimulatorApp.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

SimulatorApp::SimulatorApp(int width, int height, int port)
: config(config_file) {

    window = initializeWindow(width, height,"QM's DL Trophy Smiulator");

    gladLoadGL(glfwGetProcAddress);

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::cout << "Check OpenGL Version: " << glVersion << std::endl;

    if ((void*)glCreateShader == nullptr) {
        throw std::runtime_error("OpenGL not actually loaded - bad.");
    };

    config.restore(window);
    auto rect = Rect::query(window);

    state = new TrophyState;

    shader = new TrophyShader(rect, config, state);
    shader->assertCompileSuccess(showError);

    receiver = new UdpReceiver(port);
}

SimulatorApp::~SimulatorApp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}

GLFWwindow* SimulatorApp::initializeWindow(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        throw std::runtime_error("Cannot restore GLFW");
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

    startTime = static_cast<float>(glfwGetTime());

    // TODO: DEV STUFF - REMOVE ASAP
    state->randomizeForDebugging();

    while (!glfwWindowShouldClose(window)) {

        buildImguiControls();

        shader->use();
        auto elapsedTime = handleElapsedTime();
        shader->draw(elapsedTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
        handleInput();

        handleUdpMessages();
    }

    config.store(window);
}

void SimulatorApp::handleInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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

void SimulatorApp::buildImguiControls() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Fragment Shader Example");
    ImGui::Text("This is a window with a fragment shader!");
    ImGui::End();

}

float SimulatorApp::handleElapsedTime() {
    currentTime = static_cast<float>(glfwGetTime());
    return currentTime - startTime;
}

// there should be many implementations, but this one is enough for now
#ifdef _WIN32
#include <windows.h>
    void SimulatorApp::showError(const std::string &message) {
        MessageBoxA(nullptr, message.c_str(), "Error", MB_OK | MB_ICONERROR);
    }
#else
    void SimulatorApp::showError(const std::string &message) {
        std::cerr << message << std::endl;
    }
#endif