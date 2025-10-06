cmake_minimum_required(VERSION 3.10)
project(game_text C)


set(CMAKE_C_FLAGS "-std=c99 -Wall")


link_directories(${CMAKE_SOURCE_DIR})


add_executable(game_text game_text.c)

target_link_libraries(game_text game m)
