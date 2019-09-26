cmake_minimum_required(VERSION 3.0.0)

# Set system env
#if (NOT DEFINED VK_SDK_PATH)
#    message(FATAL_ERROR "VK_SDK_PATH is not defined")
#endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

### Handle dependencies

## GLFW
# Look for precompiled glfw library
find_package(glfw3 3.2 QUIET)

# Otherwise, build from source
if (NOT ${glfw3_FOUND})
    # Build glfw from source
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glfw)
    set(GLFW3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/glfw/include)
endif()
# build vulkanMemoryAllocator from source


if (UNIX)
    set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -pthread -Wno-missing-braces -g")
    set (CMAKE_CXX_DEBUG_FLAGS ${CMAKE_CXX_FLAGS} "")
    set (CMAKE_CXX_RELEASE_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
elseif(MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
endif()

include_directories(
    ${GLFW3_INCLUDE_DIR}
    )

include_directories(
    thirdparty/gl3w/include
    thirdparty/imgui
    )

add_library(imgui
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_impl_opengl3.cpp
    thirdparty/imgui/imgui_impl_glfw.cpp
    #thirdparty/imgui/filedialog/filedialog.cc
    )

add_library(gl3w thirdparty/gl3w/src/gl3w.c)

add_executable(main src/main.cpp src/logreader.cpp src/logrender.cpp)
target_link_libraries(main imgui glfw gl3w)

enable_testing()
add_executable(logtest src/test.cpp src/logreader.cpp)
add_test(NAME LogReader COMMAND logtest "${CMAKE_SOURCE_DIR}/raw/Log-20190902-175620 Segment.csv")
    
