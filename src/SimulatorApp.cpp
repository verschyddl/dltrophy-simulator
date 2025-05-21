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

    trophy = new Trophy();
    state = new ShaderState(trophy);

    shader = new TrophyShader(rect, config, state);
    shader->assertCompileSuccess(showError);

    receiver = new UdpReceiver(port);

    initializeKeyMap();
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

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto app = static_cast<SimulatorApp*>(
                glfwGetWindowUserPointer(window)
        );
        app->handleKeyInput(key, scancode, action, mods);
    });

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

    // TODO: replace when we have a better idea for development
    state->randomize();

    trophy->printDebug();

    shader->use();

    while (!glfwWindowShouldClose(window)) {

        buildImguiControls();

        auto elapsedTime = handleElapsedTime();
        shader->render(elapsedTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
        handleMouseInput();

        handleUdpMessages();
    }

    config.store(window);
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
        std::cerr << message << std::endl;
        std::exit(1);
    }
#else
    void SimulatorApp::showError(const std::string &message) {
        std::cerr << message << std::endl;
    }
#endif

void SimulatorApp::handleKeyInput(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && !keyDown[key]) {
        keyDown[key] = true;
    }
    else if (action == GLFW_RELEASE && keyDown[key]) {
        try {
            auto& callback = keyMap.at(key);
            callback(mods);
        }
        catch (const std::out_of_range&) {}
    }
}

void SimulatorApp::initializeKeyMap() {
    keyMap = {{
        GLFW_KEY_ESCAPE,
        [this](int mods) {
            glfwSetWindowShouldClose(window, true);
        }
    }, {
        GLFW_KEY_G,
        [this](int mods) {
            toggle(state->options.showGrid);
        }
    }, {
        GLFW_KEY_D,
        [this](int mods) {
            state->params.ledSize *= 1.05;
        }
              }, {
        GLFW_KEY_F,
        [this](int mods) {
            state->params.ledSize /= 1.05;
        }
    }, {
        GLFW_KEY_E,
        [this](int mods) {
            state->params.ledExponent -= 0.01;
        }
    }, {
        GLFW_KEY_R,
        [this](int mods) {
            state->params.ledExponent += 0.01;
        }
    }};
}

void SimulatorApp::handleMouseInput() {
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
