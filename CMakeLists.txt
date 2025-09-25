cmake_minimum_required(VERSION 3.16)
project(PongGame C)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(PongGame src/main.c)

target_link_libraries(PongGame ${SDL2_LIBRARIES})
