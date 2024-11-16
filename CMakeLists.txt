cmake_minimum_required(VERSION 3.31)

project(engineTest)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED true)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(Game main.cpp)

if(MSVC)
target_link_libraries(Game PRIVATE winmm)
endif()

add_subdirectory(raylib)
add_subdirectory(GameFolder)
add_subdirectory(imgui)

target_include_directories(Game PUBLIC raylib/src)
target_link_libraries(Game PRIVATE raylib)

file(COPY "./Assets" DESTINATION ${CMAKE_BINARY_DIR})