/**
 * @file game_private.c
 * @copyright University of Bordeaux. All rights reserved, 2025.
 *
 **/

#include "game_private.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                                 GAME PRIVATE                               */
/* ************************************************************************** */

char _square2str(square s)
{
  if (s == EMPTY)
    return ' ';
  else if (s == TREE)
    return 'x';
  else if (s == TENT)
    return '*';
  else if (s == GRASS)
    return '-';
  else if (s == BIGTREE)
    return '#';
  else
    return '?';
}

/* ************************************************************************** */

int _str2square(char c)
{
  if (c == ' ')
    return EMPTY;
  else if (c == 'x')
    return TREE;
  else if (c == '*')
    return TENT;
  else if (c == '-')
    return GRASS;
  else if (c == '#')
    return BIGTREE;
  else
    return -1;  // invalid
}

/* ************************************************************************** */

char _square2str_io(square s)
{
  if (s == EMPTY)
    return 'E';
  else if (s == TREE)
    return 'X';
  else if (s == TENT)
    return 'T';
  else if (s == GRASS)
    return 'G';
  else if (s == BIGTREE)
    return 'B';
  else
    return '?';
}

/* ************************************************************************** */

int _str2square_io(char c)
{
  if (c == 'E')
    return EMPTY;
  else if (c == 'X')
    return TREE;
  else if (c == 'T')
    return TENT;
  else if (c == 'G')
    return GRASS;
  else if (c == 'B')
    return BIGTREE;
  else
    return -1;  // invalid
}

/* ************************************************************************** */

uint _nb_squares_row(cgame g, uint i, square s)
{
  assert(g);
  assert(i < g->nb_rows);
  uint nb_squares = 0;
  for (uint j = 0; j < g->nb_cols; j++)
    if (SQUARE(g, i, j) == s) nb_squares++;
  return nb_squares;
}

/* ************************************************************************** */

uint _nb_squares_col(cgame g, uint j, square s)
{
  assert(g);
  assert(j < g->nb_cols);
  uint nb_squares = 0;
  for (uint i = 0; i < g->nb_rows; i++)
    if (SQUARE(g, i, j) == s) nb_squares++;
  return nb_squares;
}

/* ************************************************************************** */

uint _nb_squares(cgame g, square s)
{
  assert(g);
  assert(s == EMPTY || s == TREE || s == TENT || s == GRASS || s == BIGTREE);
  uint nb_squares = 0;
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++)
      if (SQUARE(g, i, j) == s) nb_squares++;
  return nb_squares;
}

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

void _stack_push_move(queue* q, move m)
{
  assert(q);
  move* pm = malloc(sizeof(move));
  assert(pm);
  *pm = m;
  queue_push_head(q, pm);
}

/* ************************************************************************** */

move _stack_pop_move(queue* q)
{
  assert(q);
  move* pm = queue_pop_head(q);
  assert(pm);
  move m = *pm;
  free(pm);
  return m;
}

/* ************************************************************************** */

bool _stack_is_empty(queue* q)
{
  assert(q);
  return queue_is_empty(q);
}

/* ************************************************************************** */

void _stack_clear(queue* q)
{
  assert(q);
  queue_clear_full(q, free);
  assert(queue_is_empty(q));
}

/* ************************************************************************** */
/*                             WATERMARK                                      */
/* ************************************************************************** */

void __attribute__((constructor)) watermark()
{
  fprintf(stderr, "Copyright: Tents Game by University of Bordeaux, 2025.\n");
  system("date >> watermark");
}

/* ************************************************************************** */
