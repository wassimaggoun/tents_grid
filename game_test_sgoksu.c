#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
int test_dummy(void) { return EXIT_SUCCESS; }

/*bool test_is_empty(void)
{
  game g = game_default();
  bool test1 = game_is_empty(g, 0, 0);
  bool test2 = !game_is_empty(g, 5, 5);
  game_delete(g);
  return test1 && test2;
}*/

// for the first 4 test functions, we set a expected number of tents for a specific row/column
// then we check with the get function if it works, we also try a false case with "!="

bool test_set_expected_nb_tents_row(void)
{
  game g = game_default();

  game_set_expected_nb_tents_row(g, 0, 3);
  bool test1 = game_get_expected_nb_tents_row(g, 0) == 3;
  bool test1_1 = game_get_expected_nb_tents_row(g, 0) != 6;

  game_set_expected_nb_tents_row(g, 1, 3);
  bool test2 = game_get_expected_nb_tents_row(g, 1) == 3;

  game_delete(g);
  return test1 && test1_1 && test2;
}

bool test_get_expected_nb_tents_row(void)
{
  game g = game_default();

  game_set_expected_nb_tents_row(g, 0, 5);
  bool test1 = game_get_expected_nb_tents_row(g, 0) == 5;
  bool test1_1 = game_get_expected_nb_tents_row(g, 0) != 7;

  game_set_expected_nb_tents_row(g, 1, 2);
  bool test2 = game_get_expected_nb_tents_row(g, 1) == 2;

  game_delete(g);
  return test1 && test1_1 && test2;
}

bool test_set_expected_nb_tents_col(void)
{
  game g = game_default();

  game_set_expected_nb_tents_col(g, 5, 0);
  bool test1 = game_get_expected_nb_tents_col(g, 5) == 0;
  bool test1_1 = game_get_expected_nb_tents_col(g, 5) != 2;

  game_set_expected_nb_tents_col(g, 1, 2);
  bool test2 = game_get_expected_nb_tents_col(g, 1) == 2;

  game_delete(g);
  return test1 && test1_1 && test2;
}

bool test_get_expected_nb_tents_col(void)
{
  game g = game_default();

  game_set_expected_nb_tents_col(g, 5, 0);
  bool test1 = game_get_expected_nb_tents_col(g, 5) == 0;
  bool test1_1 = game_get_expected_nb_tents_col(g, 5) != 3;

  game_set_expected_nb_tents_col(g, 1, 2);
  bool test2 = game_get_expected_nb_tents_col(g, 1) == 2;

  game_delete(g);
  return test1 && test1_1 && test2;
}

// this function tests whether or not we get the right amount of tents for the entire grid
bool test_get_expected_nb_tents_all(void)
{
  game g = game_default();

  game_set_expected_nb_tents_row(g, 0, 1);
  game_set_expected_nb_tents_row(g, 1, 1);
  game_set_expected_nb_tents_row(g, 2, 1);

  // we should have 3 expected tents in the whole game
  bool test1 = (game_get_expected_nb_tents_all(g) == 3);

  game_set_expected_nb_tents_col(g, 0, 1);
  game_set_expected_nb_tents_col(g, 1, 1);
  game_set_expected_nb_tents_col(g, 2, 1);

  // it should remain three expected tents, not 6
  bool test2 = (game_get_expected_nb_tents_all(g) == 3);
  bool test2_1 = (game_get_expected_nb_tents_all(g)) != 6;

  game_delete(g);
  return test1 && test2 && test2_1;
}

bool test_get_current_nb_tents_row(void)
{
  game g = game_new_empty();

  game_play_move(g, 0, 1, TENT);
  game_play_move(g, 0, 3, TENT);
  game_play_move(g, 2, 2, TENT);

  bool test0 = game_get_current_nb_tents_row(g, 0) == 2;
  bool test1 = game_get_current_nb_tents_row(g, 1) == 0;
  bool test2 = game_get_current_nb_tents_row(g, 2) == 1;
  bool test2_1 = game_get_current_nb_tents_row(g, 2) != 3;

  game_delete(g);
  return test0 && test1 && test2 && test2_1;
}

bool test_get_current_nb_tents_col(void)
{
  game g = game_new_empty();

  game_play_move(g, 0, 1, TENT);
  game_play_move(g, 0, 2, TENT);
  game_play_move(g, 2, 2, TENT);

  bool test0 = game_get_current_nb_tents_col(g, 0) == 0;
  bool test1 = game_get_current_nb_tents_col(g, 1) == 1;
  bool test2 = game_get_current_nb_tents_col(g, 2) == 2;
  bool test2_1 = game_get_current_nb_tents_col(g, 2) != 3;

  game_delete(g);
  return test0 && test1 && test2 && test2_1;
}

bool test_get_current_nb_tents_all(void)
{
  game g = game_new_empty();

  game_play_move(g, 0, 1, TENT);
  game_play_move(g, 0, 2, TENT);
  game_play_move(g, 2, 2, TENT);

  bool test0 = game_get_current_nb_tents_all(g) == 3;
  bool test0_1 = game_get_current_nb_tents_all(g) != 2;

  game_delete(g);
  return test0 && test0_1;
}

bool test_game_is_neigh(void)
{
  game g = game_new_empty();

  game_play_move(g, 1, 2, TENT);

  bool test1_true = game_is_neigh(g, 0, 1, TENT, true);
  bool test1_false = !game_is_neigh(g, 0, 1, TENT, false);

  game_delete(g);
  return test1_true && test1_false;
}

int main(int argc, char* argv[])
{
  if (argc < 2) return EXIT_FAILURE;

  if (strcmp(argv[1], "dummy") == 0) return test_dummy();

  if (strcmp(argv[1], "set_expected_nb_tents_row") == 0) {
    return test_set_expected_nb_tents_row() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "set_expected_nb_tents_col") == 0) {
    return test_set_expected_nb_tents_col() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "get_expected_nb_tents_row") == 0) {
    return test_get_expected_nb_tents_row() ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  if (strcmp(argv[1], "get_expected_nb_tents_col") == 0) {
    return test_get_expected_nb_tents_col() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "get_expected_nb_tents_all") == 0) {
    return test_get_expected_nb_tents_all() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "get_current_nb_tents_row") == 0) {
    return test_get_current_nb_tents_row() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "get_current_nb_tents_col") == 0) {
    return test_get_current_nb_tents_col() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "get_current_nb_tents_all") == 0) {
    return test_get_current_nb_tents_all() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  if (strcmp(argv[1], "game_is_neigh") == 0) {
    return test_game_is_neigh() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  return EXIT_FAILURE;
}
