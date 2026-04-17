/**
 * @file wrapper.js
 * @brief Game Binding to JavaScript (based on Emscripten & Wasm)
 * @author aurelien.esnard@u-bordeaux.fr
 * @copyright University of Bordeaux. All rights reserved, 2026.
 **/

#include <emscripten.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "game_tools.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
// #include "game_tools.h"

/* ******************** Game WASM API ******************** */

EMSCRIPTEN_KEEPALIVE
game new_default(void) { return game_default(); }

EMSCRIPTEN_KEEPALIVE
void delete(game g) { game_delete(g); }

EMSCRIPTEN_KEEPALIVE
void play_move(game g, uint i, uint j, square s) { game_play_move(g, i, j, s); }

EMSCRIPTEN_KEEPALIVE
bool check_move(game g, uint i, uint j, square s) { return game_check_move(g, i, j, s); }

EMSCRIPTEN_KEEPALIVE
void restart(game g) { game_restart(g); }

EMSCRIPTEN_KEEPALIVE
void fill_grass(game g) { game_fill_grass(g); }

EMSCRIPTEN_KEEPALIVE
bool won(game g) { return game_won(g); }

EMSCRIPTEN_KEEPALIVE
square get_square(cgame g, uint i, uint j) { return game_get_square(g, i, j); }

EMSCRIPTEN_KEEPALIVE
uint get_expected_nb_tents_row(cgame g, uint i) { return game_get_expected_nb_tents_row(g, i); }

EMSCRIPTEN_KEEPALIVE
uint get_expected_nb_tents_col(cgame g, uint j) { return game_get_expected_nb_tents_col(g, j); }

EMSCRIPTEN_KEEPALIVE
uint get_expected_nb_tents_all(cgame g) { return game_get_expected_nb_tents_all(g); }

EMSCRIPTEN_KEEPALIVE
uint get_current_nb_tents_row(cgame g, uint i) { return game_get_current_nb_tents_row(g, i); }

EMSCRIPTEN_KEEPALIVE
uint get_current_nb_tents_col(cgame g, uint j) { return game_get_current_nb_tents_col(g, j); }

EMSCRIPTEN_KEEPALIVE
uint get_current_nb_tents_all(cgame g) { return game_get_current_nb_tents_all(g); }

EMSCRIPTEN_KEEPALIVE
uint nb_rows(cgame g) { return game_nb_rows(g); }

EMSCRIPTEN_KEEPALIVE
uint nb_cols(cgame g) { return game_nb_cols(g); }

EMSCRIPTEN_KEEPALIVE
bool is_wrapping(cgame g) { return game_is_wrapping(g); }

EMSCRIPTEN_KEEPALIVE
void undo(game g) { game_undo(g); }

EMSCRIPTEN_KEEPALIVE
void redo(game g) { game_redo(g); }

/* Add more function wrappers here, if needed... */

EMSCRIPTEN_KEEPALIVE
bool solve(game g) { game_restart(g); return game_solve(g); }

EMSCRIPTEN_KEEPALIVE
game new_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees, bool solution) { return game_random(nb_rows, nb_cols, wrapping, nb_trees, nb_bigtrees, solution); }

// EOF
