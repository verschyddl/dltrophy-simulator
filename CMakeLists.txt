cmake_minimum_required(VERSION 3.22)
project(dltrophy_simulator)

set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

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

# copy the shader sources into the build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
     FILES_MATCHING PATTERN "*.glsl"
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/UdpReceiver.cpp
        src/UdpReceiver.h
        src/SimulatorApp.cpp
        src/SimulatorApp.h
        src/TrophyShader.cpp
        src/TrophyShader.h
)

target_sources(${PROJECT_NAME} PRIVATE
        thirdparty/glad/src/gl.c
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(${PROJECT_NAME}
        PUBLIC ${imgui_SOURCE_DIR}
        PUBLIC ${imgui_SOURCE_DIR}/backends
        PUBLIC thirdparty/glad/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
        MinimalSocket
)

add_subdirectory(tools)
