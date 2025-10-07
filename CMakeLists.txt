cmake_minimum_required(VERSION 3.16)
project(PongGame C)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(PongGame src/main.c)

target_link_libraries(PongGame m SDL2::SDL2 SDL2_ttf::SDL2_ttf)
