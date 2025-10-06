/**
 * @file game_aux.h
 * @brief Auxiliarry Game Functions.
 * @details See @ref index for further details.
 * @copyright University of Bordeaux. All rights reserved, 2025.
 *
 **/

#ifndef __GAME_AUX_H__
#define __GAME_AUX_H__
#include <stdbool.h>

#include "game.h"

typedef enum {
  HERE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT
} direction;

/**
 * @brief Prints a game as text on the standard output stream.
 * @details The different squares EMPTY, TREE, TENT, and GRASS are respectively
 * displayed with the following characters ' ', 'x', '*', '-'.
 * Here is the attempted output for the default game:
@verbatim
   01234567
   --------
0 |x    xx | 1
1 |        | 2
2 |      x | 1
3 |x  x    | 2
4 |     x  | 2
5 |   x    | 0
6 |        | 1
7 | xx  xx | 3
   --------
   30212121
@endverbatim
 * @param g the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
void game_print(cgame g);

/**
 * @brief Creates the default game.
 * @details See the description of the default game in @ref index.
 * @details
 *https://www.chiark.greenend.org.uk/~sgtatham/puzzles/js/tents.html#8x8de#911105660705777
 * @return the created game
 **/
game game_default(void);

/**
 * @brief Creates the default game solution.
 * @details See the description of the default game in @ref index. Note that
 * this game must no longer have empty squares and have grass squares instead.
 * @details
 *https://www.chiark.greenend.org.uk/~sgtatham/puzzles/js/tents.html#8x8de#911105660705777
 * @return the created game
 **/
game game_default_solution(void);

/**
 * @brief Gets the coordinate of the adjacent square in a given direction.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction
 * @param[out] pi_next the row index of the adjacent square (output)
 * @param[out] pj_next the column index of the adjacent square (output)
 * @pre @p g must be a valid pointer toward a game structure
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if the adjacent square is inside the grid, false otherwise
 */
bool game_get_ajacent_square(cgame g, uint i, uint j, direction d,
                             uint* pi_next, uint* pj_next);

/**
 * @brief Determines if a neighboring square has a given value.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s the square value
 * @param diag if true, include diagonal neighbors; otherwise, only check
 * orthogonal neighbors
 * @pre @p g is a valid pointer to a game structure
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if any neighbor has the given value @p s, false otherwise
 */
bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag);

#endif  // __GAME_AUX_H__
