cmake_minimum_required(VERSION 3.10)
project(game_text C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-std=c99 -Wall -g")


link_directories(${CMAKE_SOURCE_DIR})


add_executable(game_text game_text.c)

target_link_libraries(game_text game m)

add_executable(game_test_zziane game_test_zziane.c)
add_test(test_zziane_dummy ./game_test_zziane dummy)

add_executable(game_test_waggoun game_test_waggoun.c)
add_test(test_waggoun_dummy ./game_test_waggoun dummy)

add_executable(game_test_sgoksu game_test_sgoksu.c)
add_test(test_sgoksu_dummy ./game_test_sgoksu dummy)