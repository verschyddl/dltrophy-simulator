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

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.0
)
FetchContent_MakeAvailable(json)

set(USE_ZLIB OFF CACHE BOOL "Enable zlib support" FORCE)
FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG v11.4.6
)
FetchContent_MakeAvailable(ixwebsocket)

set(BUILD_MinimalCppSocket_SAMPLES OFF CACHE BOOL "Build MinimalSocket samples" FORCE)
FetchContent_Declare(
        min_sock
        GIT_REPOSITORY https://github.com/andreacasalino/Minimal-Socket
        GIT_TAG        master
)
FetchContent_MakeAvailable(min_sock)


# copy the shader sources into the build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
     FILES_MATCHING PATTERN "*.glsl"
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/SimulatorApp.cpp
        src/SimulatorApp.h
        src/TrophyShader.cpp
        src/TrophyShader.h
        src/Config.cpp
        src/Config.h
        src/Trophy.h
        src/inputHelpers.h
        src/geometryHelpers.h
        src/debug.h

        src/messages/WebSocketListener.h
        src/messages/UdpListener.h
        src/messages/UdpInterpreter.h
        src/messages/timeFormat.h
        src/monitor/PerformanceMonitor.h
        src/monitor/Observables.h
)

target_sources(${PROJECT_NAME} PRIVATE
        thirdparty/glad/src/gl.c
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
        src/messages
        src/monitor
        thirdparty/glad/include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgui_SOURCE_DIR}/misc/cpp/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
        glm::glm
        nlohmann_json::nlohmann_json
        ixwebsocket
        MinimalSocket
)


# also, some tools for testing, debugging or wholesome jux
add_subdirectory(tools)
