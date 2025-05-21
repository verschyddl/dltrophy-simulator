//
// Created by qm210 on 21.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_INPUTHELPERS_H
#define DLTROPHY_SIMULATOR_INPUTHELPERS_H

#include <vector>
#include <functional>
#include <cstdint>
#include "GLFW/glfw3.h"
#include "SimulatorApp.h"

typedef uint16_t KeyCode;

typedef std::unordered_map<
    KeyCode,
    std::function<void(int)>
> KeyMap;

inline void toggle(bool& flag) {
    flag = !flag;
}

#endif //DLTROPHY_SIMULATOR_INPUTHELPERS_H
