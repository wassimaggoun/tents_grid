
cmake_minimum_required(VERSION 3.10)
project(game_text C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-std=c99 -Wall -g")


link_directories(${CMAKE_SOURCE_DIR})


add_executable(game_text game_text.c)

target_link_libraries(game_text game m)

add_executable(game_test_zziane game_test_zziane.c)
target_link_libraries(game_test_zziane game m)
add_test(test_zziane_dummy ./game_test_zziane dummy)
add_test(test_zziane_game_check_move ./game_test_zziane game_check_move)


add_executable(game_test_waggoun game_test_waggoun.c)
target_link_libraries(game_test_waggoun game m)
add_test(test_waggoun_dummy ./game_test_waggoun dummy)
add_test(test_waggoun_game_new_empty ./game_test_waggoun game_new_empty)
add_test(test_waggoun_game_delete ./game_test_waggoun game_delete)

add_executable(game_test_sgoksu game_test_sgoksu.c)
target_link_libraries(game_test_sgoksu game m)
add_test(test_sgoksu_dummy ./game_test_sgoksu dummy)

