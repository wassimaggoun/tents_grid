#include "game.h"
#include "game_aux.h"
#include "game_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {

  if (argc < 7) {
    fprintf(stderr, "Usage: %s <nb_rows> <nb_cols> <wrapping> <nb_trees> <nb_bigtrees> <solution> [<filename>]\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  uint nb_rows = (uint)atoi(argv[1]);
  uint nb_cols = (uint)atoi(argv[2]);
  bool wrapping = (bool)atoi(argv[3]);
  uint nb_trees = (uint)atoi(argv[4]);
  uint nb_bigtrees = (uint)atoi(argv[5]);
  bool with_solution = (bool)atoi(argv[6]);

  char* filename = (argc >= 8) ? argv[7] : NULL;

  srand(time(NULL));

  game g = game_random(nb_rows, nb_cols, wrapping, nb_trees, nb_bigtrees, with_solution);

  if (g == NULL) {
    fprintf(stderr, "Erreur critique lors de la génération du jeu.\n");
    return EXIT_FAILURE;
  }

  game_print(g);

  if (filename != NULL) {
    game_save(g, filename);
    printf("Jeu sauvegardé dans : %s\n", filename);
  }

  game_delete(g);

  return EXIT_SUCCESS;
}