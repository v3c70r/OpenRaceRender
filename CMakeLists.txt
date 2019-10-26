cmake_minimum_required(VERSION 3.0.0)
project(OpenRaceRender)

#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(FFMPEG REQUIRED)
include_directories(${FFMPEG_INCLUDE_DIR})

#find_package(MPV REQUIRED)
#include_directories(${MPV_INCLUDE_DIR})


if (UNIX)
    set (CMAKE_CXX_COMPILER "g++-8")
    set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -pthread -Wno-missing-braces -g")
    set (CMAKE_CXX_DEBUG_FLAGS ${CMAKE_CXX_FLAGS} "")
    set (CMAKE_CXX_RELEASE_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
    if (NOT APPLE)
        set (CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "/usr/lib/gcc/x86_64-linux-gnu/8/") # force gcc8's dir
    endif()
elseif(MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
endif()

include_directories(
    ${GLFW3_INCLUDE_DIR}
    )

include_directories(
    thirdparty
    thirdparty/gl3w/include
    thirdparty/imgui
    src
    )

add_library(imgui
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_impl_opengl3.cpp
    thirdparty/imgui/imgui_impl_glfw.cpp
    thirdparty/imgui/filedialog/filedialog.cc
    thirdparty/imgui/imgui_plot/imgui_plot.cpp
    )

add_library(gl3w thirdparty/gl3w/src/gl3w.c)

#add_library(videoplayer src/videoplayer.cpp)
#target_link_libraries(videoplayer ${FFMPEG_LIBRARIES})

add_executable(OpenRaceRender src/main.cpp src/logreader.cpp src/logrender.cpp src/videoplayermpv.cpp)

target_link_libraries(OpenRaceRender imgui glfw gl3w)

if (NOT APPLE)
find_library(CPPFS NAMES "libstdc++fs.a" PATHS "/usr/lib/gcc/x86_64-linux-gnu/8/")
#find_library(MPV NAMES "libmpv.so" PATHS "${CMAKE_SOURCE_DIR}/thirdparty/libmpv/" "/usr/local/lib")
find_package(MPV REQUIRED)
target_link_libraries(OpenRaceRender ${CPPFS})
include_directories(${MPV_INCLUDE_PATH})
else()
find_library(MPV NAMES "libmpv.dylib" PATHS "/usr/local/lib")
endif()


enable_testing()
add_executable(logtest src/test/testreader.cpp src/logreader.cpp)

#add_executable(videoplayertest src/test/testvideoplayer.cpp)
#target_link_libraries(videoplayertest videoplayer)

add_test(NAME LogReader COMMAND logtest "${CMAKE_SOURCE_DIR}/raw/Log-20190902-175620 Segment.csv")

########################
# Test different libraries
add_executable(mpvtest src/test/testmpv.cpp)

target_link_libraries(OpenRaceRender ${MPV_LIBRARY})

target_link_libraries(mpvtest ${MPV_LIBRARY})
#add_test(NAME VideoDecoder COMMAND decodertest "${CMAKE_SOURCE_DIR}/raw/sample.mp4")

# test video decoder 
#add_executable(testFFmpeg src/videodecoder_test.cpp)
#target_link_libraries(testFFmpeg ${FFMPEG_LIBRARIES})
