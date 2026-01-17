#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col)
{
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

  g->grid = malloc(game_nb_rows(g) * sizeof(square*));
  if (!g->grid) {
    free(g);
    return NULL;
  }

  for (uint i = 0; i < game_nb_rows(g); i++) {
    g->grid[i] = malloc(game_nb_cols(g) * sizeof(square));
    if (!g->grid[i]) {
      for (uint k = 0; k < i; k++) {
        free(g->grid[k]);
      }
      free(g->grid);
      free(g);
      return NULL;
    }
  }

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      uint index = i * game_nb_cols(g) + j;
      g->grid[i][j] = squares[index];
    }
  }

  g->expected_nb_tents_row = malloc(game_nb_rows(g) * sizeof(uint));
  if (!g->expected_nb_tents_row) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      free(g->grid[i]);
    }
    free(g->grid);
    free(g);
    return NULL;
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    g->expected_nb_tents_row[i] = nb_tents_row[i];
  }

  g->expected_nb_tents_col = malloc(game_nb_cols(g) * sizeof(uint));
  if (!g->expected_nb_tents_col) {
    free(g->expected_nb_tents_row);
    for (uint i = 0; i < game_nb_rows(g); i++) {
      free(g->grid[i]);
    }
    free(g->grid);
    free(g);
    return NULL;
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    g->expected_nb_tents_col[j] = nb_tents_col[j];
  }
  g->undo_stack = queue_new();
  g->redo_stack = queue_new();
  if (!g->undo_stack || !g->redo_stack) {
    if (g->undo_stack) queue_free(g->undo_stack);
    if (g->redo_stack) queue_free(g->redo_stack);
    free(g->expected_nb_tents_col);
    free(g->expected_nb_tents_row);
    for (uint i = 0; i < g->height; i++) free(g->grid[i]);
    free(g->grid);
    free(g);
    return NULL;
  }

  return g;
}

game game_new_empty(void)
{
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

game game_copy(cgame g)
{
  game gc = malloc(sizeof(struct game_s));
  assert(gc != NULL);

  gc->width = game_nb_cols(g);
  gc->height = game_nb_rows(g);
  gc->wrapping = g->wrapping;

  gc->expected_nb_tents_row = malloc(game_nb_rows(g) * sizeof(uint));
  gc->expected_nb_tents_col = malloc(game_nb_cols(g) * sizeof(uint));
  assert(gc->expected_nb_tents_row != NULL);
  assert(gc->expected_nb_tents_col != NULL);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    gc->expected_nb_tents_row[i] = g->expected_nb_tents_row[i];
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    gc->expected_nb_tents_col[j] = g->expected_nb_tents_col[j];
  }

  gc->grid = malloc(game_nb_rows(g) * sizeof(square*));
  assert(gc->grid != NULL);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    gc->grid[i] = malloc(game_nb_cols(g) * sizeof(square));
    assert(gc->grid[i] != NULL);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      gc->grid[i][j] = g->grid[i][j];
    }
  }
  gc->undo_stack = queue_new();
  gc->redo_stack = queue_new();
  assert(gc->undo_stack != NULL);
  assert(gc->redo_stack != NULL);

  return gc;
}

bool game_equal(cgame g1, cgame g2)
{
  if (!g1 || !g2) return false;

  if (g1->width != g2->width || g1->height != g2->height || g1->wrapping != g2->wrapping) return false;

  if ((g1->expected_nb_tents_row == NULL) != (g2->expected_nb_tents_row == NULL)) return false;

  if (g1->expected_nb_tents_row) {
    for (uint i = 0; i < g1->height; i++) {
      if (g1->expected_nb_tents_row[i] != g2->expected_nb_tents_row[i]) return false;
    }
  }

  if ((g1->expected_nb_tents_col == NULL) != (g2->expected_nb_tents_col == NULL)) return false;

  if (g1->expected_nb_tents_col) {
    for (uint j = 0; j < g1->width; j++) {
      if (g1->expected_nb_tents_col[j] != g2->expected_nb_tents_col[j]) return false;
    }
  }

  for (uint i = 0; i < g1->height; i++) {
    for (uint j = 0; j < g1->width; j++) {
      if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) return false;
    }
  }

  return true;
}

void game_delete(game g)
{
  if (!g) return;

  if (g->grid) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      free(g->grid[i]);
    }
    free(g->grid);
  }

  free(g->expected_nb_tents_row);
  free(g->expected_nb_tents_col);

  if (g->undo_stack) queue_free_full(g->undo_stack, free);
  if (g->redo_stack) queue_free_full(g->redo_stack, free);

  free(g);
}

void game_set_square(game g, uint i, uint j, square s)
{
  if (!g) return;
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) return;
  if (s != EMPTY && s != TREE && s != TENT && s != GRASS && s != BIGTREE) return;

  g->grid[i][j] = s;
}

square game_get_square(cgame g, uint i, uint j)
{
  if (!g) return EMPTY;
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) return EMPTY;

  return g->grid[i][j];
}

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents)
{
  if (g == NULL) {
    return;
  }
  if (i >= game_nb_rows(g)) {
    return;
  }
  g->expected_nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents)
{
  if (g == NULL) {
    return;
  }
  if (j >= game_nb_cols(g)) {
    return;
  }
  g->expected_nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i)
{
  if(g == NULL) return;
  if(!g->expected_nb_tents_row || i < game_nb_rows(g)) return;

  return g->expected_nb_tents_row[i];
}

uint game_get_expected_nb_tents_col(cgame g, uint j)
{
  if(g == NULL) return;
  if(!g->expected_nb_tents_col || j < game_nb_rows(g)) return;

  return g->expected_nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g)
{
  if(g == NULL) return;
  if(!g->expected_nb_tents_row) return;
  uint res = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    res += game_get_expected_nb_tents_row(g, i);
  }
  return res;
}

uint game_get_current_nb_tents_row(cgame g, uint i)
{
  assert(g);
  assert(i < game_nb_rows(g));

  uint res = 0;
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == TENT) {
      res++;
    }
  }
  return res;
}

uint game_get_current_nb_tents_col(cgame g, uint j)
{
  assert(g);
  assert(j < game_nb_cols(g));

  uint res = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_square(g, i, j) == TENT) {
      res++;
    }
  }
  return res;
}

uint game_get_current_nb_tents_all(cgame g)
{
  assert(g != NULL);
  uint cpt = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TENT) cpt++;
    }
  }
  return cpt;
}

void game_play_move(game g, uint i, uint j, square s)
{
  assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));

  if (g->grid[i][j] == TREE || g->grid[i][j] == BIGTREE) {
    return;
  }
  if (s != EMPTY && s != GRASS && s != TENT) {
    return;
  }

  move* m = malloc(sizeof(move));
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

bool game_check_move(cgame g, uint i, uint j, square s)
{
  if (!g) return false;

  if (i >= g->height || j >= g->width) {
    return false;
  }

  if (s != EMPTY && s != GRASS && s != TENT) {
    return false;
  }

  square current = g->grid[i][j];
  if (current == TREE || current == BIGTREE) {
    return false;
  }

  if (s == TREE || s == BIGTREE) {
    return false;
  }

  return true;
}

bool game_won(cgame g)
{
  if (!g) return false;

  uint total_tents = 0;
  uint required_tents = 0;

  // regle 2
  for (uint i = 0; i < game_nb_rows(g); i++)
    if (game_get_current_nb_tents_row(g, i) != game_get_expected_nb_tents_row(g, i)) return false;

  for (uint j = 0; j < game_nb_cols(g); j++)
    if (game_get_current_nb_tents_col(g, j) != game_get_expected_nb_tents_col(g, j)) return false;

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);

      if (s == TENT) {
        total_tents++;

        // regle 1
        if (game_is_neigh(g, i, j, TENT, true)) return false;

        // regle 4
        uint trees = game_nb_neigh(g, i, j, TREE, false) + game_nb_neigh(g, i, j, BIGTREE, false);

        if (trees == 0) return false;
      }

      if (s == TREE) {
        required_tents++;

        if (!game_is_neigh(g, i, j, TENT, false)) return false;
      }

      if (s == BIGTREE) {
        if (game_nb_neigh(g, i, j, TENT, false) != 2) return false;
        required_tents += 2;
      }
    }
  }

  // regle 3
  if (total_tents != required_tents) return false;

  return true;
}

void game_fill_grass(game g)
{
  assert(g);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        game_set_square(g, i, j, GRASS);
      }
    }
  }
}

void game_restart(game g)
{
  if (!g) return;

  if (g->undo_stack) queue_clear_full(g->undo_stack, free);
  if (g->redo_stack) queue_clear_full(g->redo_stack, free);

  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      square s = g->grid[i][j];

      if (s != TREE && s != BIGTREE) {
        g->grid[i][j] = EMPTY;
      }
    }
  }
}