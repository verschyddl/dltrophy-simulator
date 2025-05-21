//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_SIMULATORAPP_H
#define DLTROPHY_SIMULATOR_SIMULATORAPP_H

// these are interconnected, keep in that order
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include "UdpReceiver.h"
#include "ShaderState.h"
#include "TrophyShader.h"
#include "Config.h"
#include "inputHelpers.h"

class SimulatorApp {
private:
    GLFWwindow* window;
    GLFWwindow* initializeWindow(int width, int height, const std::string& title);
    static void handleWindowError(int error, const char* description);
    void buildImguiControls();

    KeyMap keyMap;
    void initializeKeyMap();
    void handleKeyInput(int key, int scancode, int action, int mods);
    std::unordered_map<KeyCode, bool> keyDown;
    void handleMouseInput();

    TrophyShader* shader;

    float startTime;
    float currentTime;
    float handleElapsedTime();

    Trophy* trophy;
    ShaderState* state;

    UdpReceiver* receiver;
    void handleUdpMessages();

    const std::string config_file = "persisted.config";
    Config config;

    static void showError(const std::string & message);

public:
    SimulatorApp(int width, int height, int port);
    ~SimulatorApp();

    void run();
};

#endif //DLTROPHY_SIMULATOR_SIMULATORAPP_H
