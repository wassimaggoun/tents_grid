#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
int test_dummy() {
  printf("NO ERREUR\n ");
  return EXIT_SUCCESS;
}

bool test_check_move() {
  // Crée le jeu par défaut
  game g = game_default();

  // Teste si on peut poser une tente ici (valide)
  bool t1 = game_check_move(g, 2, 3, TENT);
  // Teste un coup interdit (ex: sur un arbre ou une tente existante)
  bool t2 = !game_check_move(g, 0, 0, TENT);
  bool t3 = !game_check_move(g, 4, 4, TENT);
  // Teste si on peut mettre de l'herbe (valide ou non)
  bool t5 = !game_check_move(g, 5, 2, GRASS);
  bool t8 = game_check_move(g, 5, 3, GRASS);
  // Teste une case hors de la grille
  bool t6 = !game_check_move(g, 100, 100, TENT);
  // Teste l'ajout interdit d'un arbre
  bool t9 = !game_check_move(g, 1, 1, TREE);

  // Nettoie la mémoire
  game_delete(g);

  // Vérifie que tous les tests sont passés
  return t1 && t2 && t3 && t5 && t6 && t8 && t9;
}

bool test_play_move() {
  // Crée un jeu vide
  game g = game_new_empty();

  // Pose une tente et vérifie si elle est bien là
  game_play_move(g, 0, 0, TENT);
  bool t1 = (game_get_square(g, 0, 0) == TENT);
  // Vérifie les compteurs de tentes (ligne, colonne, total)
  bool t2 = (game_get_current_nb_tents_row(g, 0) == 1);
  bool t3 = (game_get_current_nb_tents_col(g, 0) == 1);
  bool t4 = (game_get_current_nb_tents_all(g) == 1);

  // Pose de l'herbe et vérifie
  game_play_move(g, 0, 1, GRASS);
  bool t5 = (game_get_square(g, 0, 1) == GRASS);
  bool t6 = (game_get_current_nb_tents_row(g, 0) == 1);
  bool t7 = (game_get_current_nb_tents_col(g, 1) == 0);

  // Efface la case (met EMPTY) et vérifie que tout est à zéro
  game_play_move(g, 0, 0, EMPTY);
  bool t8 = (game_get_square(g, 0, 0) == EMPTY);
  bool t9 = (game_get_current_nb_tents_row(g, 0) == 0);
  bool t10 = (game_get_current_nb_tents_col(g, 0) == 0);
  bool t11 = (game_get_current_nb_tents_all(g) == 0);

  // Nettoie la mémoire
  game_delete(g);

  return t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8 && t9 && t10 && t11;
}

bool test_default_solution() {
  // Charge la solution du jeu par défaut
  game g = game_default_solution();

  // Vérifie que le jeu a bien été créé
  bool t1 = (g != NULL);

  // Vérifie qu'il n'y a plus aucune case vide (tout est rempli)
  bool t2 = true;
  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        t2 = false;
        break;
      }
    }
    if (!t2)
      break;
  }

  // Vérifie que la partie est gagnée
  bool t3 = game_won(g);

  // Nettoie la mémoire
  game_delete(g);

  return t1 && t2 && t3;
}

bool test_default() {
  // Charge le jeu par défaut (non résolu)
  game g = game_default();

  bool t1 = (g != NULL);

  bool hastree = false;
  bool hasempty = false;
  bool hastentougrass = false;

  // Parcourt la grille pour voir ce qu'elle contient
  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      square s = game_get_square(g, i, j);
      if (s == TREE)
        hastree = true;
      if (s == EMPTY)
        hasempty = true;
      if (s == TENT || s == GRASS)
        hastentougrass = true;
    }
  }

  // Vérifie qu'il y a des arbres et du vide, mais pas encore de tentes ou d'herbe
  bool t2 = hastree;
  bool t3 = hasempty;
  bool t4 = !hastentougrass;

  // Vérifie que la partie n'est pas encore gagnée
  bool t5 = !game_won(g);

  // Nettoie la mémoire
  game_delete(g);

  return t1 && t2 && t3 && t4 && t5;
}

bool test_fill_grass() {
  // Crée un jeu vide
  game g = game_new_empty();

  // Place manuellement un arbre, une tente et de l'herbe
  game_set_square(g, 0, 0, TREE);
  game_set_square(g, 0, 1, TENT);
  game_set_square(g, 0, 2, GRASS);

  // Remplit tout le reste avec de l'herbe
  game_fill_grass(g);

  // Vérifie que nos cases manuelles n'ont pas changé
  bool t1 = (game_get_square(g, 0, 0) == TREE);
  bool t2 = (game_get_square(g, 0, 1) == TENT);
  bool t3 = (game_get_square(g, 0, 2) == GRASS);

  // Vérifie que toutes les autres cases sont devenues de l'herbe
  bool t4 = true;
  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if (i == 0 && (j == 0 || j == 1 || j == 2)) {
        continue; // On saute nos cases manuelles
      }
      if (game_get_square(g, i, j) != GRASS) {
        t4 = false;
        break;
      }
    }
    if (!t4)
      break;
  }

  // Vérifie qu'il n'y a toujours qu'une seule tente
  bool t5 = (game_get_current_nb_tents_all(g) == 1);

  // Nettoie la mémoire
  game_delete(g);

  return t1 && t2 && t3 && t4 && t5;
}

bool test_get_square(void) {
  // Crée un jeu vide
  game g = game_new_empty();

  // Place des éléments à différents endroits
  game_set_square(g, 0, 0, TREE);
  game_set_square(g, 1, 2, TENT);
  game_set_square(g, 3, 3, GRASS);

  // Vérifie qu'on récupère bien les bonnes valeurs
  bool t1 = (game_get_square(g, 0, 0) == TREE);
  bool t2 = (game_get_square(g, 1, 2) == TENT);
  bool t3 = (game_get_square(g, 3, 3) == GRASS);

  // Vérifie qu'une case non touchée est vide
  bool t4 = (game_get_square(g, 5, 5) == EMPTY);

  // Modifie une case et revérifie
  game_set_square(g, 5, 5, TENT);
  bool t5 = (game_get_square(g, 5, 5) == TENT);

  // Nettoie la mémoire
  game_delete(g);

  return t1 && t2 && t3 && t4 && t5;
}

bool test_print(void) {
  // Charge le jeu
  game g1 = game_default();

  // Affiche le jeu dans la console (pour vérifier visuellement)
  game_print(g1);

  bool t1 = true;

  // Nettoie la mémoire
  game_delete(g1);

  return t1;
}

int main(int argc, char* argv[])

{
  if (argc != 2) {
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "dummy") == 0) {
    return test_dummy();
  }

  if (strcmp(argv[1], "game_check_move") == 0) {
    if (test_check_move()) {
      printf("TEST game_check_move : OK\n");
      return EXIT_SUCCESS;
    } else {
      printf("TEST game_check_move : ECHEC\n");
      return EXIT_FAILURE;
    }
  }
  if (strcmp(argv[1], "game_play_move") == 0)
    return test_play_move() ? EXIT_SUCCESS : EXIT_FAILURE;

  if (strcmp(argv[1], "game_default_solution") == 0)
    return test_default_solution() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_default") == 0)
    return test_default() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_fill_grass") == 0)
    return test_fill_grass() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_get_square") == 0)
    return test_get_square() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_print") == 0)
    return test_print() ? EXIT_SUCCESS : EXIT_FAILURE;
  printf("ERREUR\n ");
  return EXIT_FAILURE;
}