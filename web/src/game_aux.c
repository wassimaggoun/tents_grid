/**
 * @file game_aux.c
 * @copyright University of Bordeaux. All rights reserved, 2025.
 *
 **/

#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"

/* ************************************************************************** */

int DIR2OFFSET[][2] = {
    [HERE] = {0, 0},      [UP] = {-1, 0},        [DOWN] = {1, 0},
    [LEFT] = {0, -1},     [RIGHT] = {0, 1},      [UP_LEFT] = {-1, -1},
    [UP_RIGHT] = {-1, 1}, [DOWN_LEFT] = {1, -1}, [DOWN_RIGHT] = {1, 1},
};

/* ************************************************************************** */

void game_print(cgame g)
{
  assert(g);
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("%d", j);
  printf("\n");
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    printf("%d |", i);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      char c = _square2str(s);
      printf("%c", c);
    }
    printf("| %d\n", game_get_expected_nb_tents_row(g, i));
  }
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("%d", game_get_expected_nb_tents_col(g, j));
  printf("\n");
}

/* ************************************************************************** */

game game_default(void)
{
  square squares[] = {
      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, TREE,  TREE,  EMPTY, /* row 0 */
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 1 */
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, /* row 2 */
      TREE,  EMPTY, TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 3 */
      EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, EMPTY, EMPTY, /* row 4 */
      EMPTY, EMPTY, TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 5 */
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 6 */
      EMPTY, TREE,  TREE,  EMPTY, EMPTY, TREE,  TREE,  EMPTY, /* row 7 */
  };
  uint nb_tents_row[] = {1, 2, 1, 2, 2, 0, 1, 3};
  uint nb_tents_col[] = {3, 0, 2, 1, 2, 1, 2, 1};
  return game_new(squares, nb_tents_row, nb_tents_col);
}

/* ************************************************************************** */

game game_default_solution(void)
{
  square squares[] = {
      TREE,  GRASS, GRASS, GRASS, TENT,  TREE,  TREE,  GRASS, /* row 0 */
      TENT,  GRASS, GRASS, GRASS, GRASS, GRASS, TENT,  GRASS, /* row 1 */
      GRASS, GRASS, TENT,  GRASS, GRASS, GRASS, TREE,  GRASS, /* row 2 */
      TREE,  GRASS, TREE,  GRASS, TENT,  GRASS, TENT,  GRASS, /* row 3 */
      TENT,  GRASS, TENT,  GRASS, TREE,  GRASS, GRASS, GRASS, /* row 4 */
      GRASS, GRASS, TREE,  GRASS, GRASS, GRASS, GRASS, GRASS, /* row 5 */
      GRASS, GRASS, GRASS, GRASS, GRASS, TENT,  GRASS, GRASS, /* row 6 */
      TENT,  TREE,  TREE,  TENT,  GRASS, TREE,  TREE,  TENT,  /* row 7 */
  };
  uint nb_tents_row[] = {1, 2, 1, 2, 2, 0, 1, 3};
  uint nb_tents_col[] = {3, 0, 2, 1, 2, 1, 2, 1};
  game g = game_new(squares, nb_tents_row, nb_tents_col);
  return g;
}

/* ************************************************************************** */

bool game_get_adjacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);

  // convert direction to offset
  int i_offset = DIR2OFFSET[d][0];
  int j_offset = DIR2OFFSET[d][1];

  // move to the next square in a given direction
  int ii = i + i_offset;
  int jj = j + j_offset;

  if (game_is_wrapping(g)) {
    ii = (ii + game_nb_rows(g)) % game_nb_rows(g);
    jj = (jj + game_nb_cols(g)) % game_nb_cols(g);
  }

  // check if next square at (ii,jj) is out of grid
  if (ii < 0 || ii >= (int)g->nb_rows) return false;
  if (jj < 0 || jj >= (int)g->nb_cols) return false;

  *pi_next = ii;
  *pj_next = jj;

  return true;
}

/* ************************************************************************** */

uint game_nb_neigh(cgame g, uint i, uint j, square s, bool diag)
{
  assert(g);
  assert(i < g->nb_rows && j < g->nb_cols);

  uint count = 0;
  uint ii, jj;
  // orthogonally
  if (game_get_adjacent_square(g, i, j, UP, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
  if (game_get_adjacent_square(g, i, j, DOWN, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
  if (game_get_adjacent_square(g, i, j, LEFT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
  if (game_get_adjacent_square(g, i, j, RIGHT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;

  // diagonally
  if (diag) {
    if (game_get_adjacent_square(g, i, j, UP_LEFT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
    if (game_get_adjacent_square(g, i, j, UP_RIGHT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
    if (game_get_adjacent_square(g, i, j, DOWN_LEFT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
    if (game_get_adjacent_square(g, i, j, DOWN_RIGHT, &ii, &jj) && SQUARE(g, ii, jj) == s) count++;
  }

  return count;
}

/* ************************************************************************** */

bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag)
{
  assert(g);
  assert(i < g->nb_rows && j < g->nb_cols);
  uint nb_neigh = game_nb_neigh(g, i, j, s, diag);
  return (nb_neigh > 0);
}

/* ************************************************************************** */
