//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_MAINWINDOW_H
#define DLTROPHY_SIMULATOR_MAINWINDOW_H

#include <GLFW/glfw3.h>
#include <string>

class MainWindow {
private:
    GLFWwindow* window;
    int width;
    int height;

public:
    MainWindow(int width, int height, const std::string& title);
    ~MainWindow();

    void run();
};

#endif //DLTROPHY_SIMULATOR_MAINWINDOW_H
