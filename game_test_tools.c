#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

bool test_game_load(void) {
  game g = game_load("../file/gameload.txt");
  assert(g);
  game g1 = game_default();
  bool test1 = game_equal(g, g1);
  game g2 = game_new_empty_ext(9, 8, true);
  bool test2 = !game_equal(g, g2);
  game_delete(g);
  game_delete(g1);
  game_delete(g2);
  return test1 && test2;
}

bool test_game_save(void) {
  game g = game_default();
  game_save(g, "../file/gamesave.txt");
  game g1 = game_load("../file/gamesave.txt");
  bool test = game_equal(g, g1);
  game_delete(g);
  game_delete(g1);
  return test;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Erreur\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "game_load") == 0)
    return test_game_load() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_save") == 0)
    return test_game_save() ? EXIT_SUCCESS : EXIT_FAILURE;

  fprintf(stderr, "Erreur\n");
  return EXIT_FAILURE;
}