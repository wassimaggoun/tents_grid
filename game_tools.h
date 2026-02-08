/**
 * @file game_tools.h
 * @brief Game Tools.
 * @details See @ref index for further details.
 * @copyright University of Bordeaux. All rights reserved, 2025.
 *
 **/

#ifndef __GAME_TOOLS_H__
#define __GAME_TOOLS_H__
#include <stdbool.h>
#include <stdio.h>

#include "game.h"

/**
 * @name Game Tools
 * @{
 */

/**
 * @brief Creates a game by loading its description in a text file.
 * @details See the file format description.
 * @param filename input filename
 * @return the loaded game (or NULL in case of error)
 **/
game game_load(char* filename);

/**
 * @brief Saves a game in a text file.
 * @details See the file format description.
 * @param g game to save
 * @param filename output filename
 **/
void game_save(cgame g, char* filename);

/**
 * @brief Generates a random game with the given attributes.
 * @details This function incrementally places random trees (including big trees) on the grid,
 * ensuring that a valid solution exists. That is, it checks that each placed tree can have its
 * tent(s) placed legally. After several attempts, the function gives up even if the requested
 * number of trees has not yet been placed, and returns the game generated so far.
 * @param nb_rows the number of rows
 * @param nb_cols the number of columns
 * @param wrapping wrapping option
 * @param nb_trees the expected number of trees
 * @param nb_bigtrees the expected number of big trees
 * @param solution if true, the generated game is the solution (tents and grass are placed), else
 * only trees are placed
 * @return the generated game
 */
game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees, bool solution);

/**
 * @}
 */

#endif // __GAME_TOOLS_H__
