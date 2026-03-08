#include "game.h"
#include "game_aux.h"
#include "game_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Usage : ./game_solve <option> <input> [<output>]\n");
    return EXIT_FAILURE;
  }

  char* option = argv[1];
  char* input = argv[2];
  char* output = NULL;
  if (argc == 4) {
    output = argv[3];
  }

  game g = game_load(input);
  if (!g) {
    fprintf(stderr, "Error loading game\n");
    return EXIT_FAILURE;
  }
  if (strcmp(option, "-s") == 0) {
    if (game_solve(g)) {
      if (output) {
        game_save(g, output);
        printf("solution saved in file %s\n", output);
        game_delete(g);
        return EXIT_SUCCESS;
      }
      printf("Solution of the game:\n");
      game_print(g);
      game_delete(g);
      return EXIT_SUCCESS;
    }

    printf("No solutiouns could be found for this game\n");
    game_delete(g);
    return EXIT_FAILURE;

  } else if (strcmp(option, "-c") == 0) {
    uint count = game_nb_solutions(g);
    if (output) {
      FILE* f = fopen(output, "w");

      if (!f) {
        fprintf(stderr, "Error opening file %s\n", output);
        game_delete(g);
        return EXIT_FAILURE;
      }

      fprintf(f, "%u\n", count);
      fclose(f);
      printf("result saved in file %s", output);
      return EXIT_SUCCESS;
    }
    printf("%u\n", count);
    game_delete(g);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Unknown option %s\n", option);
    game_delete(g);
    return EXIT_FAILURE;
  }
}