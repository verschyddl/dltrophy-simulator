//
// Created by qm210 on 10.05.2025.
//

#include <stdexcept>
#include <iostream>
#include "MainWindow.h"

MainWindow::MainWindow(int width, int height, const std::string &title)
: width(width), height(height) {

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

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::cout << "Check OpenGL Version: " << glVersion << std::endl;
}

MainWindow::~MainWindow() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void MainWindow::run() {

    while (!glfwWindowShouldClose(window)) {

        // TODO: render something

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

