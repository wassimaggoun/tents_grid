#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

int main(void)
{
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "Erreur : échec de la création du jeu.\n");
    return EXIT_FAILURE;
  }

  char c;
  unsigned int i, j;

  while (!game_won(g)) {
    game_print(g);

    if (scanf(" %c", &c) != 1) {
      break;
    }

    if (c == 'h') {
      printf("Commandes :\n");
      printf("  h             : afficher cette aide\n");
      printf("  t i j         : tenter une tente à la case (i, j)\n");
      printf("  g i j         : poser de l’herbe à la case (i, j)\n");
      printf("  e i j         : vider la case (i, j)\n");
      printf("  r             : réinitialiser le jeu\n");
      printf("  f             : remplir tout d’herbe\n");
      printf("  q             : quitter\n");
    } else if (c == 't' || c == 'g' || c == 'e') {
      if (scanf("%u %u", &i, &j) == 2) {
        if (c == 't') {
          game_play_move(g, i, j, TENT);
        } else if (c == 'g') {
          game_play_move(g, i, j, GRASS);
        } else {  // c == 'e'
          game_play_move(g, i, j, EMPTY);
        }
      }
    } else if (c == 'r') {
      game_restart(g);
    } else if (c == 'f') {
      game_fill_grass(g);
    } else if (c == 'q') {
      printf("shame\n");
      game_print(g);
      game_delete(g);
      return EXIT_SUCCESS;
    } else if (c == 'z') {
      game_undo(g);
    } else if (c == 'y') {
      game_redo(g);
    } else {
      printf("Commande inconnue : %c\n", c);
    }
  }

  game_print(g);
  printf("congratulation\n");

  game_delete(g);
  return EXIT_SUCCESS;
}