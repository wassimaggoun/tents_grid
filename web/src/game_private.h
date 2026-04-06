/**
 * @file game_private.h
 * @brief Private Game Functions.
 * @details See @ref index for further details.
 * @copyright University of Bordeaux. All rights reserved, 2025.
 **/

#ifndef __GAME_PRIVATE_H__
#define __GAME_PRIVATE_H__

#include <stdbool.h>

#include "game.h"
#include "game_aux.h"
#include "queue.h"

/* ************************************************************************** */
/*                             DATA TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Move structure.
 * @details This structure is used to save the game history.
 */
struct move_s {
  uint i, j;
  square old, new;
};

typedef struct move_s move;

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

/** push a move in the stack */
void _stack_push_move(queue* q, move m);

/** pop a move from the stack */
move _stack_pop_move(queue* q);

/** test if the stack is empty */
bool _stack_is_empty(queue* q);

/** clear all the stack */
void _stack_clear(queue* q);

/* ************************************************************************** */
/*                             PRIVATE ROUTINES                               */
/* ************************************************************************** */

/**
 * @brief convert a square into a character
 *
 * @param s a square
 * @return the corresponding character, '?' otherwise
 */
char _square2str(square s);

/**
 * @brief convert a character into square
 *
 * @param c character
 * @return the corresponding square value, -1 otherwise
 */
int _str2square(char c);

char _square2str_io(square s);
int _str2square_io(char c);

/**
 * @brief get the number of squares with a certain value in a row
 *
 * @param g the game
 * @param i row index
 * @param s the square value to look for
 * @return the number of squares found
 */
uint _nb_squares_row(cgame g, uint i, square s);

/**
 * @brief get the number of squares with a certain value in a column
 *
 * @param g the game
 * @param j column index
 * @param s the square value to look for
 * @return the number of squares found
 */
uint _nb_squares_col(cgame g, uint j, square s);

/**
 * @brief get the number of squares with a certain value in the board
 *
 * @param g the game
 * @param s the square value to look for
 * @return the number of squares found
 */
uint _nb_squares(cgame g, square s);

#endif  // __GAME_PRIVATE_H__
