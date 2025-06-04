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

inline bool outsideVec4Rect(double posX, double posY, glm::vec4 rect) {
    return (posX < rect.x ||
            posX >= rect.x + rect.z ||
            posY < rect.y ||
            posY >= rect.y + rect.w);
}

namespace ImGuiHelper {
    inline bool SlidersVec3(const std::string& label,
                     float* x, float minX, float maxX,
                     float* y, float minY, float maxY,
                     float* z, float minZ, float maxZ,
                     float itemWidth = 0.f
                     ) {
        bool changed = false;
        auto name = label.c_str();
        ImGui::PushID(name);
        if (itemWidth) {
            ImGui::PushItemWidth(itemWidth);
        }

        changed |= ImGui::SliderFloat(
                "##X", x, minX, maxX
        );
        ImGui::SameLine();
        changed |= ImGui::SliderFloat(
                "##Y", y, minY, maxY
        );
        ImGui::SameLine();
        changed |= ImGui::SliderFloat(
                "##Z", z, minZ, maxZ
        );
        ImGui::SameLine();
        ImGui::Text(name);

        if (itemWidth) {
            ImGui::PopItemWidth();
        }
        ImGui::PopID();

        return changed;
    }
}

#endif //DLTROPHY_SIMULATOR_INPUTHELPERS_H
