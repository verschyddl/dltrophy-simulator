//
// Created by qm210 on 21.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_INPUTHELPERS_H
#define DLTROPHY_SIMULATOR_INPUTHELPERS_H

#include <vector>
#include <functional>
#include <cstdint>

#include <GLFW/glfw3.h>
#include <imgui.h>

typedef uint16_t KeyCode;

typedef std::unordered_map<
    KeyCode,
    std::function<void(int)>
> KeyMap;

inline void toggle(bool& flag) {
    flag = !flag;
}

namespace ImGuiHelper {
    inline void SlidersVec3(const std::string& label,
                     float* x, float minX, float maxX,
                     float* y, float minY, float maxY,
                     float* z, float minZ, float maxZ,
                     float item_width = 0.f
                     ) {
        auto name = label.c_str();
        ImGui::PushID(name);
        if (item_width) {
            ImGui::PushItemWidth(item_width);
        }
        ImGui::SliderFloat("##X", x, minX, maxX);
        ImGui::SameLine();
        ImGui::SliderFloat("##Y", y, minY, maxY);
        ImGui::SameLine();
        ImGui::SliderFloat("##Z", z, minZ, maxZ);
        ImGui::SameLine();
        ImGui::Text(name);
        if (item_width) {
            ImGui::PopItemWidth();
        }
        ImGui::PopID();
    }
}

#endif //DLTROPHY_SIMULATOR_INPUTHELPERS_H
