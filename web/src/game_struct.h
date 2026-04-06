/**
 * @file game_struct.h
 * @brief Private Game Structure.
 * @copyright University of Bordeaux. All rights reserved, 2025.
 **/

#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#include <stdbool.h>

#include "game.h"
#include "queue.h"

/* ************************************************************************** */
/*                             DATA TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Game structure.
 * @details This is an opaque data type.
 */
struct game_s {
  uint nb_rows;       /**< number of rows in the game */
  uint nb_cols;       /**< number of columns in the game */
  uint* nb_tents_row; /**< the expected number of tents in each row */
  uint* nb_tents_col; /**< the expected number of tents in each col */
  square* squares;    /**< the grid of squares */
  bool wrapping;      /**< the wrapping option */
  queue* undo_stack;  /**< stack to undo moves */
  queue* redo_stack;  /**< stack to redo moves */
};

/* ************************************************************************** */
/*                                MACRO                                       */
/* ************************************************************************** */

#define INDEX(g, i, j) ((i) * (g->nb_cols) + (j))
#define SQUARE(g, i, j) ((g)->squares[(INDEX(g, i, j))])

#endif  // __GAME_STRUCT_H__
