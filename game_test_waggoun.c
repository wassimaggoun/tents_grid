#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

#define SIZE DEFAULT_SIZE

int test_dummy(void) { return EXIT_SUCCESS; }
/* *********************************************************** */
// tester for game_new_empty
bool test_game_new_empty(void)
{
  game g = game_new_empty();
  if (g == NULL) {
    return false;
  }
  for (uint i = 0; i < SIZE; i++) {  // checking if all squares are empty
    for (uint j = 0; j < SIZE; j++) {
      if (game_get_square(g, i, j) != EMPTY) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
      }
    }
    // checking if the expected nb of tents for each row/col is set to zero
    if (game_get_expected_nb_tents_row(g, i) != 0 || game_get_expected_nb_tents_col(g, i) != 0) {
      fprintf(stderr, "Error\n");
      game_delete(g);
      return false;
    }
  }
  game_delete(g);
  return true;
}
/* *********************************************************** */
// tester for game_delete
bool test_game_delete(void)
{
  game g = game_new_empty();
  if (g == NULL) {
    fprintf(stderr, "Error\n");
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */

bool test_game_set_square(void)
{
  game g = game_new_empty();
  if (g == NULL) {
    fprintf(stderr, "Error\n");
    return false;
  }

  // we set squares with all possible values in random spots and check if there are unexcpected behavior
  game_set_square(g, 0, 0, TREE);
  if (game_get_square(g, 0, 0) != TREE) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  game_set_square(g, 0, 1, TENT);
  if (game_get_square(g, 0, 1) != TENT) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  game_set_square(g, 2, 2, GRASS);
  if (game_get_square(g, 2, 2) != GRASS) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  game_set_square(g, 2, 2, EMPTY);
  if (game_get_square(g, 2, 2) != EMPTY) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  game_set_square(g, 4, 4, BIGTREE);
  if (game_get_square(g, 4, 4) != BIGTREE) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  // trying the corner
  game_set_square(g, game_nb_rows(g) - 1, game_nb_cols(g) - 1, TREE);
  if (game_get_square(g, SIZE - 1, SIZE - 1) != TREE) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  if (game_get_square(g, 4, 5) != EMPTY) {
    fprintf(stderr, "Error\n");
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */
// tester for game_new
bool test_game_new(void)
{
  square squares[SIZE * SIZE];
  uint tents_row[SIZE];
  uint tents_col[SIZE];
  for (uint i = 0; i < SIZE * SIZE; i++) {
    squares[i] = EMPTY;
  }

  squares[0] = TREE;                  // top left corner
  squares[1] = TENT;                  // next col
  squares[SIZE + 2] = GRASS;          // next row
  squares[(SIZE * SIZE - 1)] = TREE;  // bottom right corner

  for (uint i = 0; i < SIZE; i++) {
    tents_row[i] = i % 2;        // 0,1,0,1,...
    tents_col[i] = (i + 1) % 2;  // 1,0,1,0,...
  }

  game g = game_new(squares, tents_row, tents_col);
  if (g == NULL) {
    fprintf(stderr, "Erreur\n");
    return false;
  }

  // verifying the value of each square in the grid
  for (uint i = 0; i < SIZE; i++) {
    for (uint j = 0; j < SIZE; j++) {
      square expected = squares[i * SIZE + j];  // getting the index in 1D array version
      if (game_get_square(g, i, j) != expected) {
        fprintf(stderr, "Erreur\n");
        game_delete(g);
        return false;
      }
    }
  }

  // verifying the expected nb_tents in each row/col

  for (uint i = 0; i < SIZE; i++) {
    if (game_get_expected_nb_tents_row(g, i) != tents_row[i]) {
      fprintf(stderr, "Erreur\n");
      game_delete(g);
      return false;
    }
    if (game_get_expected_nb_tents_col(g, i) != tents_col[i]) {
      fprintf(stderr, "Erreur\n");
      game_delete(g);
      return false;
    }
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */

bool test_game_copy(void)
{
  uint nb_rows = 9;
  uint nb_cols = 8;
  bool wrapping = true;
  square squares[nb_rows * nb_cols];
  uint tents_row[nb_rows];
  uint tents_col[nb_cols];

  // filling the array squares
  for (uint i = 0; i < nb_rows * nb_cols; i++) {
    squares[i] = (i % 4 == 0) ? TREE : EMPTY;  // placing trees in squares with index that is a multiple of 4 (arbitrary choice)
  }

  // setting random values  to check if they are correctly copied

  squares[0] = TREE;
  squares[1] = BIGTREE;
  squares[3] = TENT;
  squares[nb_cols + 2] = GRASS;
  squares[nb_rows * nb_cols - 1] = TREE;

  for (uint i = 0; i < nb_rows; i++) tents_row[i] = i % 2;  // 0,1,0,1,0,1...

  for (uint j = 0; j < nb_cols; j++) tents_col[j] = (j + 1) % 2;  // 1,0,1,0,1,0...

  game g1 = game_new_ext(nb_rows, nb_cols, squares, tents_row, tents_col, wrapping);
  if (!g1) return false;

  // on créer une copie de g1
  game g2 = game_copy(g1);
  if (g2 == NULL) {
    fprintf(stderr, "Erreur\n");
    game_delete(g1);
    return false;
  }

  // checking equality for all structure fields including arrays (no undo/redo)

  if (game_nb_rows(g1) != game_nb_rows(g2) || game_nb_cols(g1) != game_nb_cols(g2)) {
    game_delete(g1);
    game_delete(g2);
    return false;
  }

  if (game_is_wrapping(g1) != game_is_wrapping(g2)) {
    game_delete(g1);
    game_delete(g2);
    return false;
  }

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) {
        game_delete(g1);
        game_delete(g2);
        return false;
      }
    }
  }

  for (uint i = 0; i < nb_rows; i++)
    if (game_get_expected_nb_tents_row(g1, i) != game_get_expected_nb_tents_row(g2, i)) {
      game_delete(g1);
      game_delete(g2);
      return false;
    }

  for (uint j = 0; j < nb_cols; j++)
    if (game_get_expected_nb_tents_col(g1, j) != game_get_expected_nb_tents_col(g2, j)) {
      game_delete(g1);
      game_delete(g2);
      return false;
    }

  game_set_square(g1, 0, 0, TENT);
  if (game_get_square(g2, 0, 0) == TENT) {
    fprintf(stderr, "Erreur\n");
    game_delete(g1);
    game_delete(g2);
    return false;
  }

  game_delete(g1);
  game_delete(g2);
  return true;
}
/* *********************************************************** */
// test for game_won
bool test_game_won(void)
{
  // Testing valid case
  game g = game_new_empty();
  game_set_expected_nb_tents_row(g, 1, 1);
  game_set_expected_nb_tents_col(g, 0, 1);
  game_set_square(g, 0, 0, TREE);
  game_play_move(g, 1, 0, TENT);

  if (!game_won(g)) {
    fprintf(stderr, "Erreur cas gagnant\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  // Testing RULE 1
  g = game_new_empty();
  game_set_expected_nb_tents_row(g, 1, 1);
  game_set_expected_nb_tents_col(g, 0, 1);
  game_set_expected_nb_tents_row(g, 2, 1);
  game_set_expected_nb_tents_col(g, 1, 1);

  game_set_square(g, 0, 0, TREE);
  game_play_move(g, 1, 0, TENT);
  game_set_square(g, 2, 2, TREE);
  game_play_move(g, 2, 1, TENT);

  if (game_won(g)) {
    fprintf(stderr, "Erreur (regle 1)\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  // TESTING RULE 2
  g = game_new_empty();
  game_set_expected_nb_tents_row(g, 0, 1);  // We will place 2 tents in this row
  game_set_expected_nb_tents_col(g, 0, 1);  // The rest will be placed correctly

  game_set_expected_nb_tents_col(g, 1, 1);
  game_set_expected_nb_tents_col(g, 4, 1);
  game_set_expected_nb_tents_row(g, 3, 1);

  game_set_square(g, 0, 0, TREE);
  game_play_move(g, 0, 1, TENT);

  game_set_square(g, 0, 3, TREE);
  game_play_move(g, 0, 4, TENT);

  game_set_square(g, 2, 0, TREE);
  game_play_move(g, 3, 0, TENT);

  if (game_won(g)) {  // should be false
    fprintf(stderr, "Erreur (regle2)\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  // TESTING RULE 3
  g = game_new_empty();
  game_set_expected_nb_tents_row(g, 0, 1);
  game_set_expected_nb_tents_col(g, 1, 0);

  game_set_square(g, 0, 0, TREE);  // We place more trees than tents
  game_set_square(g, 3, 2, TREE);
  game_play_move(g, 0, 1, TENT);
  if (game_won(g)) {  // should be false
    fprintf(stderr, "Erreur (regle3)\n");
    game_delete(g);
    return false;
  }
  game_set_expected_nb_tents_row(g, 3, 2);
  game_set_expected_nb_tents_col(g, 3, 1);
  game_set_expected_nb_tents_col(g, 5, 1);

  game_play_move(g, 3, 3, TENT);
  game_play_move(g, 3, 5, TENT);  // more tents than trees (using the same game)
  if (game_won(g)) {              // should be false
    fprintf(stderr, "Erreur (regle3)\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  // TESTING RULE 4
  g = game_new_empty();
  game_set_expected_nb_tents_row(g, 0, 1);
  game_set_expected_nb_tents_col(g, 2, 1);
  game_set_square(g, 0, 0, TREE);
  game_play_move(g, 0, 2, TENT);  // We plpace a tent that is not orthogonally adjacent to any tree
  if (game_won(g)) {              // should be false
    fprintf(stderr, "Erreur  (regle 4).\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  return true;
}
/* *********************************************************** */
// test for game_won (V2 version)
bool test_game_won_wrap_bigtree()
{
  // case 1 : BIGTREE
  game g = game_new_empty();
  game_set_expected_nb_tents_row(g, 0, 1);
  game_set_expected_nb_tents_row(g, 2, 1);
  game_set_expected_nb_tents_col(g, 1, 2);

  game_set_square(g, 0, 0, TREE);
  game_set_square(g, 2, 2, BIGTREE);
  game_play_move(g, 0, 1, TENT);
  game_play_move(g, 2, 1, TENT);  // only one tent adjacent to bigtree

  if (game_won(g)) {  // should be false
    fprintf(stderr, "Erreur bigtree\n");
    game_delete(g);
    return false;
  }
  // we correct it (winning case)
  game_set_expected_nb_tents_col(g, 3, 1);  // we update the excpected nb of tents in order to add a new tent
  game_set_expected_nb_tents_row(g, 2, 2);
  game_play_move(g, 2, 3, TENT);
  if (!game_won(g)) {  // should be true
    fprintf(stderr, "Erreur cas gagnant\n");
    game_delete(g);
    return false;
  }
  game_delete(g);
  // wrapping case
  g = game_new_empty_ext(9, 8, true);  // wrapping is ON
  game_set_expected_nb_tents_row(g, 0, 2);
  game_set_expected_nb_tents_col(g, 0, 1);
  game_set_expected_nb_tents_col(g, 7, 1);

  // we put a tent on each border (adjacent when wrapping is ON)
  game_set_square(g, 0, 1, TREE);
  game_set_square(g, 0, 6, TREE);
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 0, 7, TENT);

  if (game_won(g)) {  // should be false
    fprintf(stderr, "Erreur wrapping\n");
    game_delete(g);
    return false;
  }
  game_delete(g);

  // We set OFF wrapping mode
  g = game_new_empty_ext(9, 8, false);
  game_set_expected_nb_tents_row(g, 0, 2);
  game_set_expected_nb_tents_col(g, 0, 1);
  game_set_expected_nb_tents_col(g, 7, 1);

  game_set_square(g, 0, 1, TREE);
  game_set_square(g, 0, 6, TREE);
  game_play_move(g, 0, 0, TENT);  // the tents are not adjacent anymore (no wrapping)
  game_play_move(g, 0, 7, TENT);

  if (!game_won(g)) {  // should be true
    fprintf(stderr, "Erreur wrapping cas gagnant\n");
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */
// test for game_equal
bool test_game_equal(void)
{
  game g1 = game_new_empty();
  if (!g1) {
    fprintf(stderr, "Erreur\n");
    return false;
  }

  game g2 = game_copy(g1);
  if (!g2) {
    fprintf(stderr, "Erreur\n");
    game_delete(g1);
    return false;
  }

  if (!game_equal(g1, g2)) {  // should be true
    fprintf(stderr, "Erreur\n");
    game_delete(g1);
    game_delete(g2);
    return false;
  }

  // checking independance (g2 shouldn't be changed as well)
  game_play_move(g2, 0, 0, TENT);
  if (game_equal(g1, g2)) {
    fprintf(stderr, "Erreur\n");
    game_delete(g1);
    game_delete(g2);
    return false;
  }

  game_delete(g1);
  game_delete(g2);
  return true;
}
/* *********************************************************** */
// test for game_restart
bool test_game_restart(void)
{
  game g = game_new_empty();
  if (!g) {
    fprintf(stderr, "Erreur\n");
    return false;
  }

  // setting random values

  game_set_square(g, 0, 0, TREE);
  game_play_move(g, 0, 1, TENT);  // grass and tents should be set to EMPTY after game_retsart
  game_play_move(g, 1, 1, GRASS);

  game_restart(g);

  if (game_get_square(g, 0, 0) != TREE) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  if (game_get_square(g, 0, 1) != EMPTY) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  if (game_get_square(g, 1, 1) != EMPTY) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */
// test for game_fet_adjacent_square
bool test_game_get_adjacent_square(void)
{
  game g = game_new_empty();
  if (!g) {
    fprintf(stderr, "Erreur\n");
    return false;
  }

  uint ni, nj;  // we'll store the adjacent square indexes in these variables

  // checking behavior with 4 directions

  if (!game_get_adjacent_square(g, 0, 0, RIGHT, &ni, &nj) || ni != 0 || nj != 1) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  if (game_get_adjacent_square(g, 0, 0, UP, &ni, &nj)) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  if (game_get_adjacent_square(g, SIZE - 1, 0, DOWN, &ni, &nj)) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  if (game_get_adjacent_square(g, 0, 0, LEFT, &ni, &nj)) {
    fprintf(stderr, "Erreur\n");
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */
// test with wrapping case, game_get_adjacent_square should return true if they are in opposing borders
bool test_game_get_adjacent_square_wrap(void)
{
  uint nb_rows = 4;
  uint nb_cols = 3;
  game g = game_new_empty_ext(nb_rows, nb_cols, true);  // wrapping enabled

  if (!g) {
    fprintf(stderr, "Erreur d'allocation de mémoire\n");
    return false;
  }

  uint ni, nj;  // store the adjacent square indexes in these variables

  // chacking with 4 directions (Should be true in all cases)

  if (!game_get_adjacent_square(g, 0, 0, LEFT, &ni, &nj) || ni != 0 || nj != nb_cols - 1) {
    game_delete(g);
    return false;
  }

  if ((!game_get_adjacent_square(g, 0, 0, UP, &ni, &nj) || ni != nb_rows - 1 || nj != 0)) {
    game_delete(g);
    return false;
  }

  if ((!game_get_adjacent_square(g, nb_rows - 1, 0, DOWN, &ni, &nj) || ni != 0 || nj != 0)) {
    game_delete(g);
    return false;
  }

  if ((!game_get_adjacent_square(g, 0, nb_cols - 1, RIGHT, &ni, &nj) || ni != 0 || nj != 0)) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}
/* *********************************************************** */

int main(int argc, char* argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Erreur\n");
    return EXIT_FAILURE;
  }

  if (strcmp("dummy", argv[1]) == 0) {
    return test_dummy();
  }

  if (strcmp(argv[1], "game_new_empty") == 0) return test_game_new_empty() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_delete") == 0) return test_game_delete() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_set_square") == 0) return test_game_set_square() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_new") == 0) return test_game_new() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_copy") == 0) return test_game_copy() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_won") == 0) return test_game_won() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_equal") == 0) return test_game_equal() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_restart") == 0) return test_game_restart() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_get_adjacent_square") == 0) return test_game_get_adjacent_square() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_get_adjacent_square_wrap") == 0) return test_game_get_adjacent_square_wrap() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_won_wrap_bigtree") == 0) return test_game_won_wrap_bigtree() ? EXIT_SUCCESS : EXIT_FAILURE;

  fprintf(stderr, "Erreur\n");
  return EXIT_FAILURE;
}