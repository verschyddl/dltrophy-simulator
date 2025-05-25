//
// Created by qm210 on 10.05.2025.
//

#include <stdexcept>
#include <iostream>

#include "SimulatorApp.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

SimulatorApp::SimulatorApp(Config config)
: config(config) {

    window = initializeWindow();
    gladLoadGL(glfwGetProcAddress);

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::cout << "Check OpenGL Version: " << glVersion << std::endl;

    if ((void*)glCreateShader == nullptr) {
        throw std::runtime_error("OpenGL not actually loaded - bad.");
    };

    glfwGetFramebufferSize(window, &area.width, &area.height);

    trophy = new Trophy();
    state = new ShaderState(trophy);
    shader = new TrophyShader(config, state);
    shader->assertSuccess(showError);

    receiver = new UdpReceiver(config.udpPort);

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

GLFWwindow* SimulatorApp::initializeWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Cannot initialize GLFW – and thus – anything.");
    }

    glfwSetErrorCallback(handleWindowError);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(
            config.windowSize.width,
            config.windowSize.height,
            awesomeTitle.c_str(),
            nullptr,
            nullptr
    );
    config.restore(window);

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
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    imguiFlags = ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_AlwaysAutoResize;

    return window;
}

void SimulatorApp::handleWindowError(int error, const char* description) {
    std::cerr << "Error: " << description << " (" << error << ")" << std::endl;
}

void SimulatorApp::run() {
    // TODO: replace when we have a better idea for development
    state->randomize();

    startTimestamp = static_cast<float>(glfwGetTime());
    currentTime = 0;
    currentFrame = 0;
    currentFps = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        handleResize();
        buildControlPanel();

        handleTime();

        shader->use();
        shader->iTime.set(currentTime);
        shader->iFrame.set(currentFrame);
        shader->iFPS.set(averageFps);
        shader->render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

        handleMouseInput();
        handleUdpMessages();

        shader->mightHotReload(config);
    }

    config.store(window);
}

void SimulatorApp::handleTime() {
    currentFrame++;
    previousTime = currentTime;
    latestTimestamp = static_cast<float>(glfwGetTime());
    currentTime = latestTimestamp - startTimestamp;
    currentFps = 1.f / (currentTime - previousTime);

    int f = currentFrame % FPS_SAMPLES;
    lastFps[f] = currentFps;
    averageFps = currentFrame < FPS_SAMPLES ? currentFps : calcAverageFps();
}

float SimulatorApp::calcAverageFps() {
    float sum = 0.;
    for (int f = 0; f < FPS_SAMPLES; f++) {
        sum += lastFps[f];
    }
    return sum / static_cast<float>(FPS_SAMPLES);
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
        GLFW_KEY_A,
        [this](int mods) {
            toggle(state->options.accumulateForever);
        }
    }, {
        GLFW_KEY_S,
        [this](int mods) {
            toggle(state->options.noStochasticVariation);
        }
    }, {
        GLFW_KEY_D,
        [this](int mods) {
            toggle(state->options.debug);
        }
    }};
}

const bool debugMouse = false;

void SimulatorApp::handleMouseInput() {
    // NOTE: there is no need for this function yet,
    // as everything mouse-ish is handled by ImGui

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (debugMouse) {
            std::cout << "Mouse Down Left @ (" << mouseX << ", " << mouseY << ")" << std::endl;
        }
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

void SimulatorApp::handleResize() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width == area.width && height == area.height) {
        return;
    }
    area.width = width;
    area.height = height;
    shader->onRectChange(area, config);
}

void SimulatorApp::buildControlPanel() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float panelMargin = 0.013 * area.width;
    float panelWidth = config.relativeRemainingWidth() * area.width - panelMargin;
    ImGui::SetNextWindowPos(
            ImVec2(area.width - panelWidth - panelMargin, panelMargin),
            ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(
            ImVec2(panelWidth, area.height - 2. * panelMargin),
            ImGuiCond_Always
    );

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImGui::Begin("Deadline Trophy Smiuluator", NULL, imguiFlags);
    ImGui::PopStyleVar();

    const int globalStyleVars = 4;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 18);

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(210.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(210.f, 0.75f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(210.f, 0.8f, 0.9f));
    if (ImGui::Button("Reload Shader")) {
        shader->reload(config);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::Checkbox("Hot Reload", &config.hotReloadShaders);

    auto reloaded = shader->lastReloadInfo();
    ImGui::SameLine();
    if (reloaded.second.empty()) {
        ImGui::Text(reloaded.first.c_str());
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), reloaded.first.c_str());
    }

    auto stop = 0.24f * panelWidth;
    ImGui::Text("Time:");
    ImGui::SameLine(stop);
    ImGui::Text( "%8.2f sec.",
                 shader->iTime.value);
    ImGui::Text("FPS:");
    ImGui::SameLine(stop);
    ImGui::Text( "%8.2f",
                 shader->iFPS.value);
    ImGui::Text("Resolution:");
    ImGui::SameLine(stop);
    ImGui::Text( "%4.0f x %4.0f (Offset: %.0f x %.0f)",
                 shader->iRect.value.z,
                 shader->iRect.value.w,
                 shader->iRect.value.x,
                 shader->iRect.value.y);

    if (ImGui::Button("Randomize LED colors")) {
        state->randomize();
    }    ImGui::SameLine();
    if (ImGui::Button("Print Debug Stuff")) {
        printDebug();
    }

    ImGui::PushItemWidth(0.32f * panelWidth);

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::PushItemWidth(0.15f * panelWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        ImGuiHelper::SlidersVec3(
                "Camera Origin",
                &state->params.camX, -1.f, +1.f,
                &state->params.camY, -1.f, +1.f,
                &state->params.camZ, -5.f, -1.f
        );
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::SliderFloat("Camera FOV",
                           &state->params.camFov,
                           0.1f, 2.1f);
        ImGui::SliderFloat("Camera Tilt",
                           &state->params.camTilt,
                           -60.f, 60.f);

    }

    if (ImGui::CollapsingHeader("Background Scenery")) {

        ImGui::SliderFloat("Fog Grading",
                           &state->params.fogGrading,
                           0.1f, 5.f);
        ImGui::SliderFloat("Background Spin",
                           &state->params.backgroundSpin,
                           0.f, 100.f);
        ImGui::SliderFloat("Synthwave Grid Thickness",
                           &state->params.floorLineWidth,
                           0.001f, 1.f);
        ImGui::SliderFloat("Synthwave Grid Exponent",
                           &state->params.floorExponent,
                           0.001f, 100.f);
        ImGui::SliderFloat("Synthwave Grid Glow",
                           &state->params.floorGrading,
                           0.001f, 20.f);

    }

    if (ImGui::CollapsingHeader("Pyramid (Trophy)", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::PushItemWidth(0.15f * panelWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        ImGuiHelper::SlidersVec3(
                "Pyramid Position",
                &state->params.pyramidX, -1.f, +1.f,
                &state->params.pyramidY, -1.f, +1.f,
                &state->params.pyramidZ, -1.f, +1.f
        );
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::SliderFloat("Pyramid Scale",
                           &state->params.pyramidScale,
                           1e-3f, 2.f);
        ImGui::SliderFloat("Pyramid Height",
                           &state->params.pyramidHeight,
                           0.01f, 2.f);
        ImGui::SliderFloat("Pyramid Rotation Angle",
                           &state->params.pyramidAngle,
                           -180.f, 180.f);
        ImGui::SliderFloat("Pyramid Angular Velocity",
                           &state->params.pyramidAngularVelocity,
                           -210.f, 210.f);
        ImGui::SliderFloat("Pyramid Epoxy Permittivity",
                           &state->params.epoxyPermittivity,
                           0.f, 200.f);

        ImGui::SliderFloat("LED size",
                           &state->params.ledSize,
                           1.e-3, 1.e-1);

    }

    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::SliderFloat("Previous Image Blend Factor",
                           &state->params.blendPreviousMixing,
                           0.f, 1.f);
        ImGui::Checkbox("Accumulate Forever (ignores Blend Factor)",
                        &state->options.accumulateForever);
        ImGui::Checkbox("No Stochastic Variation (indeed bit nonsense)",
                        &state->options.noStochasticVariation);

    }

    ImGui::PopItemWidth();

    if (!reloaded.second.empty()) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(210.f, 0.6f, 0.7f));
        ImGui::TextWrapped(reloaded.second.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar(globalStyleVars);
    ImGui::End();
}

void SimulatorApp::printDebug() const {
    trophy->printDebug();

    auto options = *reinterpret_cast<const int32_t*>(&state->options);
    std::cout << "[Debug State] int options = " << options << " -> 32 bit: ";
    for (int i = 31; i >= 0; --i) {
        auto bit = (options >> i) & 1;
        std::cout << bit;
        if (i % 8 == 0) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}