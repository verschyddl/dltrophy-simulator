cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

project(dltrophy_simulator)

find_package(OpenGL REQUIRED)
include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# cf. https://github.com/andreacasalino/Minimal-Socket?tab=readme-ov-file#cmake-support
set(BUILD_MinimalCppSocket_SAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        min_sock
        GIT_REPOSITORY https://github.com/andreacasalino/Minimal-Socket
        GIT_TAG        master
)
FetchContent_MakeAvailable(min_sock)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.0
)

FetchContent_MakeAvailable(json)


# copy the shader sources into the build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
     FILES_MATCHING PATTERN "*.glsl"
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/UdpReceiver.h
        src/SimulatorApp.cpp
        src/SimulatorApp.h
        src/TrophyShader.cpp
        src/TrophyShader.h
        src/Config.cpp
        src/Config.h
        src/Trophy.h
        src/inputHelpers.h
        src/UdpInterpreter.h
)

target_sources(${PROJECT_NAME} PRIVATE
        thirdparty/glad/src/gl.c
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
#        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(${PROJECT_NAME}
        PUBLIC thirdparty/glad/include
        # PUBLIC ${glm_SOURCE_DIR} # <-- #include <glm/glm.hpp> still invalid?
        PUBLIC ${imgui_SOURCE_DIR}
        PUBLIC ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
        glm::glm
        MinimalSocket
        nlohmann_json::nlohmann_json
)


# also, some tools for testing, debugging or wholesome jux
add_subdirectory(tools)
