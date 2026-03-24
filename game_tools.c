#include "game_tools.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

// Saves a game given as an argument in the file "filename" according to the provided format
void game_save(cgame g, char* filename) {
  assert(g);
  assert(filename);

  uint height = game_nb_rows(g);
  uint width = game_nb_cols(g);

  FILE* f = fopen(filename, "w"); // Opening the file in writing mode
  if (!f)
    return;

  fprintf(f, "%u %u %d \n", height, width, game_is_wrapping(g));
  // Writing the expected number of tents in each row.
  for (uint i = 0; i < height; i++) {
    uint x = game_get_expected_nb_tents_row(g, i);
    fprintf(f, "%u ", x);
  }
  fprintf(f, "\n");
  // Writing the expected number of tents in each column.
  for (uint j = 0; j < width; j++) {
    uint y = game_get_expected_nb_tents_col(g, j);
    fprintf(f, "%u ", y);
  }
  fprintf(f, "\n");

  char c = 'E';
  for (uint i = 0; i < height; i++) {
    for (uint j = 0; j < width; j++) {
      square s = game_get_square(g, i, j);
      switch (s) {
      case EMPTY: c = 'E'; break;
      case TREE: c = 'X'; break;
      case TENT: c = 'T'; break;
      case GRASS: c = 'G'; break;
      case BIGTREE: c = 'B'; break;
      default: break;
      }
      fprintf(f, "%c ", c);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

/***********************************************************************************************/
game game_load(char* filename) {
  FILE* f = fopen(filename, "r");
  if (!f) {
    return NULL;
  }
  uint nb_rows, nb_cols;
  int wrapping_int;
  if (fscanf(f, "%u %u %d", &nb_rows, &nb_cols, &wrapping_int) != 3) {
    fclose(f);
    return NULL;
  }
  bool wrapping = wrapping_int != 0;

  uint* nb_tents_rows = malloc(nb_rows * sizeof(uint));
  uint* nb_tents_cols = malloc(nb_cols * sizeof(uint));
  square* squares = malloc(nb_rows * nb_cols * sizeof(square));

  if (!nb_tents_rows || !nb_tents_cols || !squares) {
    goto error;
  }

  for (uint i = 0; i < nb_rows; i++) {
    if (fscanf(f, "%u", &nb_tents_rows[i]) != 1)
      goto error;
  }

  for (uint i = 0; i < nb_cols; i++) {
    if (fscanf(f, "%u", &nb_tents_cols[i]) != 1)
      goto error;
  }

  char c;
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (fscanf(f, " %c", &c) != 1) {
        goto error;
      }
      square s = EMPTY;
      switch (c) {
      case 'E': s = EMPTY; break;
      case 'X': s = TREE; break;
      case 'T': s = TENT; break;
      case 'G': s = GRASS; break;
      case 'B': s = BIGTREE; break;
      default: goto error;
      }
      squares[i * nb_cols + j] = s;
    }
  }

  game g = game_new_ext(nb_rows, nb_cols, squares, nb_tents_rows, nb_tents_cols, wrapping);
  free(nb_tents_rows);
  free(nb_tents_cols);
  free(squares);
  fclose(f);
  return g;

error:
  free(nb_tents_rows);
  free(nb_tents_cols);
  free(squares);
  fclose(f);
  return NULL;
}

/***********************************************************************************************/
static int opposite_direction(direction d)
{
  switch (d) {
    case UP:
      return DOWN;
    case DOWN:
      return UP;
    case LEFT:
      return RIGHT;
    case RIGHT:
      return LEFT;
    case UP_LEFT:
      return DOWN_RIGHT;
    case UP_RIGHT:
      return DOWN_LEFT;
    case DOWN_LEFT:
      return UP_RIGHT;
    case DOWN_RIGHT:
      return UP_LEFT;
    case HERE:
      return HERE;
  }
  return -1;  // to avoid warning
}

static bool try_to_place_tree(game g, bool big)
{
  uint nb_rows = game_nb_rows(g);
  uint nb_cols = game_nb_cols(g);
  uint i_tree = rand() % nb_rows;
  uint j_tree = rand() % nb_cols;
  if (game_get_square(g, i_tree, j_tree) != EMPTY) return false;  // already occupied
  direction dir = (rand() % 4) + 1;                               // UP,DOWN,LEFT,RIGHT
  uint i_tent, j_tent;
  bool ok = game_get_adjacent_square(g, i_tree, j_tree, dir, &i_tent, &j_tent);
  if (!ok) return false;                                           // out of bounds
  if (game_get_square(g, i_tent, j_tent) != EMPTY) return false;   // already occupied
  if (game_is_neigh(g, i_tent, j_tent, TENT, true)) return false;  // adjacent to another tent

  // for big tree, check square on opposite side for the second tent
  uint i_tent2, j_tent2;
  if (big) {
    direction opposite_dir = opposite_direction(dir);
    bool ok2 = game_get_adjacent_square(g, i_tree, j_tree, opposite_dir, &i_tent2, &j_tent2);
    if (!ok2) return false;
    if (game_get_square(g, i_tent2, j_tent2) != EMPTY) return false;   // already occupied
    if (game_is_neigh(g, i_tent2, j_tent2, TENT, true)) return false;  // adjacent to another tent
  }

  // place tree & first tent
  game_set_square(g, i_tree, j_tree, TREE);
  game_set_expected_nb_tents_row(g, i_tent, game_get_expected_nb_tents_row(g, i_tent) + 1);
  game_set_expected_nb_tents_col(g, j_tent, game_get_expected_nb_tents_col(g, j_tent) + 1);
  game_set_square(g, i_tent, j_tent, TENT);

  // place big tree & second tent
  if (big) {
    game_set_square(g, i_tree, j_tree, BIGTREE);
    game_set_expected_nb_tents_row(g, i_tent2, game_get_expected_nb_tents_row(g, i_tent2) + 1);
    game_set_expected_nb_tents_col(g, j_tent2, game_get_expected_nb_tents_col(g, j_tent2) + 1);
    game_set_square(g, i_tent2, j_tent2, TENT);
  }

  return true;
}

/* ************************************************************************** */

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees,
                 bool solution)
{
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  assert(g);

  // try to place big trees first
  uint num_bigtree = 0;
  uint nb_bigtree_tries = 0;
  while (num_bigtree < nb_bigtrees) {
    if (nb_bigtree_tries >= 10 * nb_bigtrees) break;  // avoid infinite loop
    nb_bigtree_tries++;
    if (try_to_place_tree(g, true)) num_bigtree++;
  }

  printf("Placed %d big trees (requested %d) after %d tries\n", num_bigtree, nb_bigtrees,
         nb_bigtree_tries);

  // then try to place trees
  uint num_tree = 0;
  uint nb_tree_tries = 0;
  while (num_tree < nb_trees) {
    if (nb_tree_tries >= 10 * nb_trees) break;  // avoid infinite loop
    nb_tree_tries++;
    if (try_to_place_tree(g, false)) num_tree++;
  }

  printf("Placed %d trees (requested %d) after %d tries\n", num_tree, nb_trees, nb_tree_tries);

  game_fill_grass(g);
  assert(game_won(g));
  if (!solution) game_restart(g);
  return g;
}

/*******************************************************************************************************/
// Fonction récursive appelé dans le solveur

static bool game_solve_rec(game g, uint idx, uint* cells, uint size, uint* count) {
  uint cols = game_nb_cols(g);
  uint rows = game_nb_rows(g);

  if (idx == size) { // condition d'arret (derniere case)
    if (count) {
      if (game_won(g)) {
        game_print(g);
        printf("\n");
        (*count)++; // si on compte les solution, on incremente le conmpteur et on retourne false (pas d'arret à la
                    // premiere solution trouvée)
      }
      return false;
    }
    return game_won(g);
  }

  // On vérifie si l'état actuel du jeu est valide

  // On check la limite de TENT pour chaque row/col
  for (uint i = 0; i < rows; i++) {
    if (game_get_current_nb_tents_row(g, i) > game_get_expected_nb_tents_row(g, i))
      return false;
  }
  for (uint j = 0; j < cols; j++) {
    if (game_get_current_nb_tents_col(g, j) > game_get_expected_nb_tents_col(g, j))
      return false;
  }

  // on traduit la position currente 1D en 2D (i,j)
  uint pos = cells[idx];
  uint i = pos / cols;
  uint j = pos % cols;

  // Si on est obligé de placer une TENT on la place sans faire une branche sans TENT (ça sert a rien)
  uint free_in_row = 0;
  uint free_in_col = 0;
  for (uint k = idx; k < size; k++) { // On compte le nombres de cases libre dans la ligne i et colonne j
    uint p = cells[k];
    uint ni = p / cols;
    uint nj = p % cols;
    if (ni == i)
      free_in_row++; // si la case libre est dans la ligne/colonne actuelle on incremente le conmpteur
    if (nj == j)
      free_in_col++;
  }

  // Nombre des TENTs restant a placer dans la row/col
  uint needed_in_row = game_get_expected_nb_tents_row(g, i) - game_get_current_nb_tents_row(g, i);
  uint needed_in_col = game_get_expected_nb_tents_col(g, j) - game_get_current_nb_tents_col(g, j);

  bool must_place = (free_in_row == needed_in_row) || (free_in_col == needed_in_col);
  if (must_place) {
    // Obligé de placer une TENT en (i,j) (espace restant = nombre de TENT restant a placer)
    if (game_check_move(g, i, j, TENT)) {
      game_set_square(g, i, j, TENT);
      if (game_solve_rec(g, idx + 1, cells, size, count))
        return true;
      game_set_square(g, i, j, EMPTY); // On annule la modification si aucune solution n'est trouvée sur cetet branche
    }
    return false;
  }

  // Pour chaque case on a deux cas : placer une TENT / ne pas placer de TENT

  // cas 1: on essaye de placer un TENT
  if (game_check_move(g, i, j, TENT)) {
    game_set_square(g, i, j, TENT);
    if (game_solve_rec(g, idx + 1, cells, size, count)) // Si une solution est trouvé sur cette branche on retourne true
      return true;
    game_set_square(g, i, j, EMPTY); // sinon on annule la modification
  }

  // cas 2: on ne place pas de TENT
  if (game_solve_rec(g, idx + 1, cells, size, count)) // on passe à la case suivante
    return true;

  return false;
}

/*********************************************************************************************************************/

// Cette fonction créé un tableau de cases adjacentes à des TREE ou BIGTREE , puis appelle la fonction récursive en
// passant le tableau en parametre
bool game_solve(game g) {
  if (!g)
    return false;
  uint rows = game_nb_rows(g);
  uint cols = game_nb_cols(g);
  uint size = rows * cols;

  uint* cells = malloc(size * sizeof(uint)); // Allocation mémoire pour le tableau de cases à tester
  uint nb_cells = 0;

  for (uint k = 0; k < size; k++) {
    uint i = k / cols;
    uint j = k % cols;
    square s = game_get_square(g, i, j);

    if (s == EMPTY && (game_is_neigh(g, i, j, TREE, false) || game_is_neigh(g, i, j, BIGTREE, false))) {
      if (game_get_expected_nb_tents_row(g, i) != 0 &&
          game_get_expected_nb_tents_col(g, j) != 0) { // on ne garde pas les cases impossibles (0 TENT excpected)
        cells[nb_cells] = k;
        nb_cells++;
      }
    }
  }
  game copy = game_copy(g); // On travail sur une copie
  bool res = game_solve_rec(copy, 0, cells, nb_cells, NULL);
  if (res) { // Si une solution est trouvée , on copie la copie dans le jeu original
    for (uint i = 0; i < rows; i++) {
      for (uint j = 0; j < cols; j++) {
        square s = game_get_square(copy, i, j);
        game_set_square(g, i, j, s);
      }
    }
    game_fill_grass(g);
  }

  free(cells);
  game_delete(copy);
  return res;
}

/*********************************************************************************************************************/
// Cette fonction calcule le nombre de solutions d'un jeu

uint game_nb_solutions(cgame g) {
  if (!g)
    return 0;

  uint rows = game_nb_rows(g);
  uint cols = game_nb_cols(g);
  uint size = rows * cols;

  uint* cells = malloc(size * sizeof(uint));
  uint nb_cells = 0;
  // On récupere les cases a tester
  for (uint k = 0; k < size; k++) {
    uint i = k / cols;
    uint j = k % cols;
    square s = game_get_square(g, i, j);

    if (s == EMPTY && (game_is_neigh(g, i, j, TREE, false) || game_is_neigh(g, i, j, BIGTREE, false))) {
      if (game_get_expected_nb_tents_row(g, i) != 0 && game_get_expected_nb_tents_col(g, j) != 0) {
        cells[nb_cells] = k;
        nb_cells++;
      }
    }
  }

  uint count = 0;
  game copy = game_copy(g);
  game_solve_rec(copy, 0, cells, nb_cells, &count); // On passe un pointeur vers count en argument pour que la fonction
                                                    // recursive ne s'arrete pas a la premiere solution

  free(cells);
  game_delete(copy);
  return count;
}

/*********************************************************************************************************************/