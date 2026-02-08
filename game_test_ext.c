#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

bool test_game_new_empty_ext(void) {
  // We create two games, with and without wrapping
  game g = game_new_empty_ext(9, 8, false);
  game g1 = game_new_empty_ext(9, 8, true);
  if (g == NULL || g1 == NULL) {
    return false;
  }
  for (uint i = 0; i < 9; i++) {
    for (uint j = 0; j < 8; j++) {
      if (game_get_square(g, i, j) != EMPTY || game_get_square(g1, i, j) != EMPTY) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        game_delete(g1);
        return false;
      }
    }
    // We check the number of columns outside the loop
    if (game_get_expected_nb_tents_row(g, i) != 0 || game_get_expected_nb_tents_row(g1, i) != 0) {
      fprintf(stderr, "Error\n");
      game_delete(g);
      game_delete(g1);
      return false;
    }
  }

  // checking the number of columns
  for (uint j = 0; j < 8; j++) {
    if (game_get_expected_nb_tents_col(g, j) != 0 || game_get_expected_nb_tents_col(g1, j) != 0) {
      fprintf(stderr, "Error\n");
      game_delete(g);
      game_delete(g1);
      return false;
    }
  }

  // We check the wrapping option
  if (game_is_wrapping(g) != false || game_is_wrapping(g1) != true) {
    game_delete(g);
    game_delete(g1);
    return false;
  }

  game_delete(g);
  game_delete(g1);
  return true;
}

bool test_game_new_ext(void) {
  // We run the same test as the V1, and we check the wrapping option
  uint nb_rows = 9;
  uint nb_cols = 8;
  square squares[nb_rows * nb_cols];
  uint tents_row[nb_rows];
  uint tents_col[nb_cols];
  bool wrapping = true;

  for (uint i = 0; i < nb_rows * nb_cols; i++) {
    squares[i] = EMPTY;
  }

  squares[0] = TREE;
  squares[nb_cols + 1] = GRASS;
  squares[(nb_rows * nb_cols - 1)] = BIGTREE;

  for (uint i = 0; i < nb_rows; i++) {
    tents_row[i] = i % 2; // 0,1,0,1...
  }
  for (uint j = 0; j < nb_cols; j++) {
    tents_col[j] = (j + 1) % 2; // 1,0,1,0...
  }

  game g = game_new_ext(nb_rows, nb_cols, squares, tents_row, tents_col, wrapping);
  if (!g)
    return false;

  if (game_nb_cols(g) != nb_cols || game_nb_rows(g) != nb_rows) {
    game_delete(g);
    return false;
  }

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      square s = squares[i * nb_cols + j];
      if (game_get_square(g, i, j) != s) {
        game_delete(g);
        return false;
      }
    }
  }

  for (uint i = 0; i < nb_rows; i++) {
    if (game_get_expected_nb_tents_row(g, i) != tents_row[i]) {
      game_delete(g);
      return false;
    }
  }

  for (uint j = 0; j < nb_cols; j++) {
    if (game_get_expected_nb_tents_col(g, j) != tents_col[j]) {
      game_delete(g);
      return false;
    }
  }

  if (game_is_wrapping(g) != wrapping) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

bool test_game_nb_rows(void) {
  // checking the function with and without wrapping
  game g1 = game_new_empty_ext(9, 8, false);
  bool test1 = (g1 != NULL) && (game_nb_rows(g1) == 9);
  game_delete(g1);

  game g2 = game_new_empty_ext(9, 8, true);
  bool test2 = (g2 != NULL) && (game_nb_rows(g2) == 9);
  game_delete(g2);

  return test1 && test2;
}

bool test_game_nb_cols(void) {
  // checking the function with and without wrapping
  game g1 = game_new_empty_ext(9, 8, false);
  bool test1 = (g1 != NULL) && (game_nb_cols(g1) == 8);
  game_delete(g1);

  game g2 = game_new_empty_ext(9, 8, true);
  bool test2 = (g2 != NULL) && (game_nb_cols(g2) == 8);
  game_delete(g2);

  return test1 && test2;
}

bool test_game_undo(void) {
  game g = game_new_empty_ext(9, 8, false);
  bool test1 = false;

  if (g != NULL) {
    game_play_move(g, 0, 0, TENT);
    square s1 = game_get_square(g, 0, 0);

    game_undo(g); // s2 should become EMPTY again
    square s2 = game_get_square(g, 0, 0);

    test1 = (s1 == TENT) && (s2 == EMPTY); // first move was placing a TENT and then undo
  }

  game_delete(g);
  return test1;
}

bool test_game_redo(void) {
  game g = game_new_empty_ext(9, 8, false);
  bool test1 = false;

  // we place a TENT, and we undo the move , then we redo (should be TENT again)
  if (g != NULL) {
    game_play_move(g, 0, 0, TENT);
    square s1 = game_get_square(g, 0, 0);

    game_undo(g);
    square s2 = game_get_square(g, 0, 0);

    game_redo(g);
    square s3 = game_get_square(g, 0, 0);

    test1 = (s1 == TENT) && (s2 == EMPTY) && (s3 == TENT);
  }
  game_delete(g);
  return test1;
}

bool test_game_is_wrapping(void) {
  game g = game_new_empty_ext(8, 8, false);
  game g2 = game_new_empty_ext(8, 8, true);
  if (!g || !g2)
    return false;

  if (game_is_wrapping(g) || !game_is_wrapping(g2))
    return false;

  game_delete(g);
  game_delete(g2);

  return true;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Erreur\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "game_undo") == 0)
    return test_game_undo() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_redo") == 0)
    return test_game_redo() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_nb_cols") == 0)
    return test_game_nb_cols() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_nb_rows") == 0)
    return test_game_nb_rows() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_new_empty_ext") == 0)
    return test_game_new_empty_ext() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_new_ext") == 0)
    return test_game_new_ext() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_is_wrapping") == 0)
    return test_game_is_wrapping() ? EXIT_SUCCESS : EXIT_FAILURE;

  fprintf(stderr, "Erreur\n");
  return EXIT_FAILURE;
}