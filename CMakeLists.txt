
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
add_test(test_zziane_game_play_move ./game_test_zziane game_play_move)
add_test(test_zziane_game_default_solution ./game_test_zziane game_default_solution)
add_test(test_zziane_game_default ./game_test_zziane game_default)
add_test(test_zziane_game_fill_grass ./game_test_zziane game_fill_grass)


add_executable(game_test_waggoun game_test_waggoun.c)
target_link_libraries(game_test_waggoun game m)
add_test(test_waggoun_dummy ./game_test_waggoun dummy)
add_test(test_waggoun_game_new_empty ./game_test_waggoun game_new_empty)
add_test(test_waggoun_game_delete ./game_test_waggoun game_delete)
add_test(test_waggoun_game_set_square ./game_test_waggoun game_set_square)
add_test(test_waggoun_game_new ./game_test_waggoun game_new)
add_test(test_waggoun_game_copy ./game_test_waggoun game_copy)
add_test(test_waggoun_game_restart ./game_test_waggoun game_restart)
add_test(test_waggoun_game_won ./game_test_waggoun game_won)
add_test(test_waggoun_game_equal ./game_test_waggoun game_equal)
add_test(test_waggoun_game_get_adjacent_square ./game_test_waggoun game_get_adjacent_square)

add_executable(game_test_sgoksu game_test_sgoksu.c)
target_link_libraries(game_test_sgoksu game m)
add_test(test_sgoksu_dummy ./game_test_sgoksu dummy)
add_test(test_sgoksu_set_expected_row ./game_test_sgoksu set_expected_nb_tents_row)
add_test(test_sgoksu_set_expected_col ./game_test_sgoksu set_expected_nb_tents_col)
add_test(test_sgoksu_get_expected_row ./game_test_sgoksu get_expected_nb_tents_row)
add_test(test_sgoksu_get_expected_col ./game_test_sgoksu get_expected_nb_tents_col)
add_test(test_sgoksu_get_expected_all ./game_test_sgoksu get_expected_nb_tents_all)
add_test(test_sgoksu_get_current_row ./game_test_sgoksu get_current_nb_tents_row)
add_test(test_sgoksu_get_current_col ./game_test_sgoksu get_current_nb_tents_col)
add_test(test_sgoksu_get_current_all ./game_test_sgoksu get_current_nb_tents_all)
add_test(test_sgoksu_game_is_neigh ./game_test_sgoksu game_is_neigh)


