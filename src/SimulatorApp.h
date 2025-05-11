//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_SIMULATORAPP_H
#define DLTROPHY_SIMULATOR_SIMULATORAPP_H

// these have a tricky interconnectedness, keep in that order
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include "UdpReceiver.h"
#include "TrophyState.h"
#include "TrophyShader.h"

class SimulatorApp {
private:
    GLFWwindow* window;
    GLFWwindow* initializeWindow(int width, int height, const std::string& title);

    TrophyShader* shader;

    TrophyState state = {};

    UdpReceiver* receiver;
    void handleUdpMessages();

public:
    SimulatorApp(int width, int height, int port);
    ~SimulatorApp();

    void run();
};

#endif //DLTROPHY_SIMULATOR_SIMULATORAPP_H
