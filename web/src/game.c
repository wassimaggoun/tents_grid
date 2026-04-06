/**
 * @file game.c
 * @copyright University of Bordeaux. All rights reserved, 2025.
 *
 **/

#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                                 GAME BASIC                                 */
/* ************************************************************************** */

game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col)
{
  return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, nb_tents_row, nb_tents_col, false);
}

/* ************************************************************************** */

game game_new_empty(void) { return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false); }

/* ************************************************************************** */

game game_copy(cgame g)
{
  game gg = game_new_ext(g->nb_rows, g->nb_cols, g->squares, g->nb_tents_row, g->nb_tents_col,
                         g->wrapping);
  return gg;
}

/* ************************************************************************** */

bool game_equal(cgame g1, cgame g2)
{
  assert(g1 && g2);

  if (g1->nb_rows != g2->nb_rows) return false;
  if (g1->nb_cols != g2->nb_cols) return false;

  for (uint i = 0; i < g1->nb_rows; i++) {
    if (g1->nb_tents_row[i] != g2->nb_tents_row[i]) return false;
  }

  for (uint j = 0; j < g1->nb_cols; j++) {
    if (g1->nb_tents_col[j] != g2->nb_tents_col[j]) return false;
  }

  for (uint i = 0; i < g1->nb_rows; i++)
    for (uint j = 0; j < g1->nb_cols; j++) {
      if (SQUARE(g1, i, j) != SQUARE(g2, i, j)) return false;
    }

  if (g1->wrapping != g2->wrapping) return false;

  return true;
}

/* ************************************************************************** */

void game_delete(game g)
{
  free(g->squares);
  free(g->nb_tents_col);
  free(g->nb_tents_row);
  queue_free_full(g->undo_stack, free);
  queue_free_full(g->redo_stack, free);
  free(g);
}

/* ************************************************************************** */

void game_set_square(game g, uint i, uint j, square s)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  assert(s == EMPTY || s == TREE || s == TENT || s == GRASS || s == BIGTREE);
  SQUARE(g, i, j) = s;
}

/* ************************************************************************** */

square game_get_square(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  return SQUARE(g, i, j);
}

/* ************************************************************************** */

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents)
{
  assert(g);
  assert(i < g->nb_rows);
  g->nb_tents_row[i] = nb_tents;
}

/* ************************************************************************** */

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents)
{
  assert(g);
  assert(j < g->nb_cols);
  g->nb_tents_col[j] = nb_tents;
}

/* ************************************************************************** */

uint game_get_expected_nb_tents_row(cgame g, uint i)
{
  assert(g);
  assert(i < g->nb_rows);
  return g->nb_tents_row[i];
}

/* ************************************************************************** */

uint game_get_expected_nb_tents_col(cgame g, uint j)
{
  assert(g);
  assert(j < g->nb_cols);
  return g->nb_tents_col[j];
}

/* ************************************************************************** */

uint game_get_expected_nb_tents_all(cgame g)
{
  assert(g);
  return _nb_squares(g, TREE) + 2 * _nb_squares(g, BIGTREE);
}

/* ************************************************************************** */

uint game_get_current_nb_tents_row(cgame g, uint i)
{
  assert(g);
  assert(i < g->nb_rows);
  return _nb_squares_row(g, i, TENT);
}

/* ************************************************************************** */

uint game_get_current_nb_tents_col(cgame g, uint j)
{
  assert(g);
  assert(j < g->nb_cols);
  return _nb_squares_col(g, j, TENT);
}

/* ************************************************************************** */

uint game_get_current_nb_tents_all(cgame g)
{
  assert(g);
  return _nb_squares(g, TENT);
}

/* ************************************************************************** */

void game_play_move(game g, uint i, uint j, square s)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  assert(s == EMPTY || s == TREE || s == TENT || s == GRASS || s == BIGTREE);
  assert(s != TREE);
  square ss = SQUARE(g, i, j);
  assert(ss != TREE);
  SQUARE(g, i, j) = s;

  // save history
  _stack_clear(g->redo_stack);
  move m = {i, j, ss, s};
  _stack_push_move(g->undo_stack, m);
}

/* ************************************************************************** */

bool game_check_move(cgame g, uint i, uint j, square s)
{
  assert(g);
  assert(s == EMPTY || s == TREE || s == TENT || s == GRASS || s == BIGTREE);

  if (i >= g->nb_rows) return false;
  if (j >= g->nb_cols) return false;

  square cs = SQUARE(g, i, j);  // current square at direction (i,j)

  if (s == TREE) return false;
  if (cs == TREE) return false;
  if (s == BIGTREE) return false;
  if (cs == BIGTREE) return false;

  return true;
}

/* ************************************************************************** */

bool game_won(cgame g)
{
  assert(g);

  // check rule 1) "No two tents are adjacent, not even diagonally."
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++)
      if ((SQUARE(g, i, j) == TENT) && game_is_neigh(g, i, j, TENT, true)) return false;

  // check rule 2.a) "In each row, the number of current tents is equal to the
  // expected number."
  for (uint i = 0; i < g->nb_rows; i++) {
    int nb_expected_tents_row = game_get_expected_nb_tents_row(g, i);
    int nb_current_tents_row = game_get_current_nb_tents_row(g, i);
    if (nb_expected_tents_row != nb_current_tents_row) return false;
  }

  // check rule 2.b) "In each column, the number of current tents is equal to
  // the expected number."
  for (uint j = 0; j < g->nb_cols; j++) {
    int nb_expected_tents_col = game_get_expected_nb_tents_col(g, j);
    int nb_current_tents_col = game_get_current_nb_tents_col(g, j);
    if (nb_expected_tents_col != nb_current_tents_col) return false;
  }

  // check rule 3) "The total number of tents is equal to the total expected
  // number."
  int nb_expected_tents = game_get_expected_nb_tents_all(g);
  int nb_current_tents = game_get_current_nb_tents_all(g);
  if (nb_expected_tents != nb_current_tents) return false;

  // check rule 4) Check tree-tent adjacency.
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++) {
      // 4.a) for tents, check there is at least one neigh tree (or big tree).
      if (SQUARE(g, i, j) == TENT) {
        if (!game_is_neigh(g, i, j, TREE, false) && !game_is_neigh(g, i, j, BIGTREE, false))
          return false;
      }

      // 4.b) for trees, check there is the correct number of neigh tents.
      uint nb_neigh_tents = game_nb_neigh(g, i, j, TENT, false);
      if ((SQUARE(g, i, j) == TREE) && nb_neigh_tents == 0) return false;
      if ((SQUARE(g, i, j) == BIGTREE) && nb_neigh_tents != 2) return false;
    }

  return true;
}

/* ************************************************************************** */

void game_fill_grass(game g)
{
  assert(g);
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++)
      if (SQUARE(g, i, j) == EMPTY) game_play_move(g, i, j, GRASS);
}

/* ************************************************************************** */

void game_restart(game g)
{
  assert(g);

  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++)
      if (SQUARE(g, i, j) != TREE && SQUARE(g, i, j) != BIGTREE) SQUARE(g, i, j) = EMPTY;

  // reset history
  _stack_clear(g->undo_stack);
  _stack_clear(g->redo_stack);
}

/* ************************************************************************** */
