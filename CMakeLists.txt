cmake_minimum_required(VERSION 3.27)
project(dltrophy_simulator)

set(CMAKE_CXX_STANDARD 20)

include(FindOpenGL)
find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# cf. https://github.com/andreacasalino/Minimal-Socket?tab=readme-ov-file#cmake-support
set(BUILD_MinimalCppSocket_SAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        min_sock
        GIT_REPOSITORY https://github.com/andreacasalino/Minimal-Socket
        GIT_TAG        master
)
FetchContent_MakeAvailable(min_sock)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/SimulatorApp.cpp
        src/SimulatorApp.h
        src/UdpReceiver.cpp
        src/UdpReceiver.h
        src/MainWindow.cpp
        src/MainWindow.h
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE OpenGL::GL
        PRIVATE glfw
        PRIVATE MinimalSocket
)

add_subdirectory(tools)

# cross-platform flags, e.g.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#if (MSVC)
#    # window specific
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#else()
#    # non-windows specific (-> need to distinguish linux further? we'll see)
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
#endif()
