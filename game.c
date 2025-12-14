#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_struct.h"

game game_new(square *squares, uint *nb_tents_row, uint *nb_tents_col) {
  if (!squares || !nb_tents_row || !nb_tents_col) {
    return NULL;
  }

  game g = malloc(sizeof(struct game_s));
  if (!g) {
    return NULL;
  }

  g->width = DEFAULT_SIZE;
  g->height = DEFAULT_SIZE;
  g->wrapping = false;

  g->grid = malloc(g->height * sizeof(square *));
  if (!g->grid) {
    free(g);
    return NULL;
  }

  for (uint i = 0; i < g->height; i++) {
    g->grid[i] = malloc(g->width * sizeof(square));
    if (!g->grid[i]) {
      for (uint k = 0; k < i; k++) {
        free(g->grid[k]);
      }
      free(g->grid);
      free(g);
      return NULL;
    }
  }

  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      uint index = i * g->width + j;
      g->grid[i][j] = squares[index];
    }
  }

  g->expected_nb_tents_row = malloc(g->height * sizeof(uint));
  if (!g->expected_nb_tents_row) {
    for (uint i = 0; i < g->height; i++) {
      free(g->grid[i]);
    }
    free(g->grid);
    free(g);
    return NULL;
  }
  for (uint i = 0; i < g->height; i++) {
    g->expected_nb_tents_row[i] = nb_tents_row[i];
  }

  g->expected_nb_tents_col = malloc(g->width * sizeof(uint));
  if (!g->expected_nb_tents_col) {
    free(g->expected_nb_tents_row);
    for (uint i = 0; i < g->height; i++) {
      free(g->grid[i]);
    }
    free(g->grid);
    free(g);
    return NULL;
  }
  for (uint j = 0; j < g->width; j++) {
    g->expected_nb_tents_col[j] = nb_tents_col[j];
  }

  return g;
}

game game_new_empty(void) {

  uint size = DEFAULT_SIZE;

  square squares[DEFAULT_SIZE * DEFAULT_SIZE];
  for (uint i = 0; i < size * size; i++) {
    squares[i] = EMPTY;
  }

  uint nb_tents_row[DEFAULT_SIZE];
  uint nb_tents_col[DEFAULT_SIZE];

  for (uint i = 0; i < size; i++) {
    nb_tents_row[i] = 0;
    nb_tents_col[i] = 0;
  }

  return game_new(squares, nb_tents_row, nb_tents_col);
}

game game_copy(cgame g) {
  game gc = malloc(sizeof(struct game_s));
  assert(gc != NULL);

  gc->width = g->width;
  gc->height = g->height;
  gc->wrapping = g->wrapping;

  gc->expected_nb_tents_row = malloc(g->height * sizeof(uint));
  gc->expected_nb_tents_col = malloc(g->width * sizeof(uint));
  assert(gc->expected_nb_tents_row != NULL);
  assert(gc->expected_nb_tents_col != NULL);

  for (uint i = 0; i < g->height; i++) {
    gc->expected_nb_tents_row[i] = g->expected_nb_tents_row[i];
  }
  for (uint j = 0; j < g->width; j++) {
    gc->expected_nb_tents_col[j] = g->expected_nb_tents_col[j];
  }

  gc->grid = malloc(g->height * sizeof(square *));
  assert(gc->grid != NULL);

  for (uint i = 0; i < g->height; i++) {
    gc->grid[i] = malloc(g->width * sizeof(square));
    assert(gc->grid[i] != NULL);
    for (uint j = 0; j < g->width; j++) {
      gc->grid[i][j] = g->grid[i][j];
    }
  }

  return gc;
}

bool game_equal(cgame g1, cgame g2) {
  if (g1->width != g2->width || g1->height != g2->height ||
      g1->wrapping != g2->wrapping)
    return false;

  for (uint i = 0; i < g1->height; i++) {
    if (g1->expected_nb_tents_row[i] != g2->expected_nb_tents_row[i])
      return false;
  }

  for (uint i = 0; i < g1->width; i++) {
    if (g1->expected_nb_tents_col[i] != g2->expected_nb_tents_col[i])
      return false;
  }

  for (uint i = 0; i < g1->height; i++) {
    for (uint j = 0; j < g1->width; j++) {
      if (g1->grid[i][j] != g2->grid[i][j])
        return false;
    }
  }
  return true;
}

void game_delete(game g) {
  if (!g)
    return;

  if (g->grid) {
    for (uint i = 0; i < g->height; i++) {
      free(g->grid[i]);
    }
    free(g->grid);
  }

  free(g->expected_nb_tents_row);
  free(g->expected_nb_tents_col);

  if (g->undo_stack)
    queue_free_full(g->undo_stack, free);
  if (g->redo_stack)
    queue_free_full(g->redo_stack, free);

  free(g);
}

void game_set_square(game g, uint i, uint j, square s) {
  if (!g)
    return;
  if (i >= g->height || j >= g->width)
    return;
  if (s != EMPTY && s != TREE && s != TENT && s != GRASS && s != BIGTREE)
    return;

  g->grid[i][j] = s;
}

square game_get_square(cgame g, uint i, uint j) {
  if (!g)
    return EMPTY;
  if (i >= g->height || j >= g->width)
    return EMPTY;

  return g->grid[i][j];
}

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
  if (g == NULL) {
    return;
  }
  if (i >= g->height) {
    return;
  }
  g->expected_nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
  if (g == NULL) {
    return;
  }
  if (j >= g->width) {
    return;
  }
  g->expected_nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i) {
  return g->expected_nb_tents_row[i];
}

uint game_get_expected_nb_tents_col(cgame g, uint j) {
  return g->expected_nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g) {
  uint res = 0;
  for (int i = 0; i < g->height; i++) {
    res += g->expected_nb_tents_row[i];
  }
  return res;
}

uint game_get_current_nb_tents_row(cgame g, uint i) {
  uint result = 0;
  for (uint j = 0; j < g->width; j++) {
    if (g->grid[i][j] == TENT)
      result++;
  }
  return result;
}

uint game_get_current_nb_tents_col(cgame g, uint j) {
  uint result = 0;
  for (uint i = 0; i < g->height; i++) {
    if (g->grid[i][j] == TENT)
      result++;
  }
  return result;
}

uint game_get_current_nb_tents_all(cgame g) {
  assert(g != NULL);
  uint cpt = 0;
  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      if (game_get_square(g, i, j) == TENT)
        cpt++;
    }
  }
  return cpt;
}

void game_play_move(game g, uint i, uint j, square s) {
  if (g == NULL || i >= g->height || j >= g->width) {
    return;
  }
  if (g->grid[i][j] == TREE || g->grid[i][j] == BIGTREE) {
    return;
  }
  if (s != EMPTY && s != GRASS && s != TENT) {
    return;
  }

  move *m = malloc(sizeof(move));
  if (!m) {
    fprintf(stderr, "Erreur d'allocation de mémoire.");
    exit(EXIT_FAILURE);
  }
  m->i = i;
  m->j = j;
  m->before = g->grid[i][j];
  m->after = s;

  g->grid[i][j] = s;
  queue_push_tail(g->undo_stack, m);
  queue_clear_full(g->redo_stack, free);
}

bool game_check_move(cgame g, uint i, uint j, square s) {
  square mtn = game_get_square(g, i, j);
  if (mtn == TREE || mtn == BIGTREE) {
    return false;
  }

  if (s == GRASS) {
    return mtn != TENT;
  }

  if (s == TENT) {
    if (mtn == TENT || mtn == TREE || mtn == BIGTREE) {
      return false;
    }
  }

  if (s == TREE || s == BIGTREE) {
    return false;
  }

  return true;
}

bool game_won(cgame g) {
  assert(g != NULL);

  uint nb_trees = 0; // compteur de TREE et BIGTREE
  uint nb_tents = 0; // compteur de TENT

  uint h = g->height;
  uint w = g->width;

  // règle 2 pour les colonnes
  for (uint j = 0; j < w; j++) {
    if (game_get_current_nb_tents_col(g, j) != g->expected_nb_tents_col[j])
      return false;
  }

  for (uint i = 0; i < h; i++) {
    // règle 2 pour les lignes
    if (game_get_current_nb_tents_row(g, i) != g->expected_nb_tents_row[i])
      return false;

    for (uint j = 0; j < w; j++) {
      square s = game_get_square(g, i, j);

      if (s == TENT) {
        nb_tents++;
        // règle 1
        if (game_is_neigh(g, i, j, TENT, true))
          return false;

        // règle 4
        if (!game_is_neigh(g, i, j, TREE, false) &&
            !game_is_neigh(g, i, j, BIGTREE, false))
          return false;
      }

      if (s == TREE || s == BIGTREE) {
        uint count_adj_tents = 0; // nombre de tents adjacentes a un arbre
        // verifie toutes les direzctions (sauf diagonales)
        if (i > 0 && game_get_square(g, i - 1, j) == TENT)
          count_adj_tents++;
        if (i + 1 < h && game_get_square(g, i + 1, j) == TENT)
          count_adj_tents++;
        if (j > 0 && game_get_square(g, i, j - 1) == TENT)
          count_adj_tents++;
        if (j + 1 < w && game_get_square(g, i, j + 1) == TENT)
          count_adj_tents++;

        if ((s == TREE && count_adj_tents != 1) ||
            (s == BIGTREE && count_adj_tents != 2)) {
          return false;
        }

        nb_trees++;
      }
    }
  }

  // règle 3
  if (nb_tents != nb_trees)
    return false;

  return true;
}

void game_fill_grass(game g) {
  for (unsigned int i = 0; i < g->height; i++) {
    for (unsigned int j = 0; j < g->width; j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        game_set_square(g, i, j, GRASS);
      }
    }
  }
}

void game_restart(game g) {
  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      if (g->grid[i][j] != TREE && g->grid[i][j] != EMPTY &&
          g->grid[i][j] != BIGTREE) {
        game_set_square(g, i, j, EMPTY);
      }
    }
  }
}