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

  char c;
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

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees, bool solution) {
    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
    if (!g) return NULL;

    direction all_dirs[4] = {UP, DOWN, LEFT, RIGHT};

    for (uint i = 0; i < nb_bigtrees; i++) {
        for (int attempt = 0; attempt < 100; attempt++) {
            uint r = rand() % nb_rows;
            uint c = rand() % nb_cols;

            if (game_get_square(g, r, c) != EMPTY) continue;

            bool placed = false;
            
            for (int d1 = 0; d1 < 3 && !placed; d1++) {
                for (int d2 = d1 + 1; d2 < 4 && !placed; d2++) {
                    uint t1_r, t1_c, t2_r, t2_c;

                    if (game_get_adjacent_square(g, r, c, all_dirs[d1], &t1_r, &t1_c) &&
                        game_get_adjacent_square(g, r, c, all_dirs[d2], &t2_r, &t2_c)) {

                        if (game_get_square(g, t1_r, t1_c) != EMPTY || 
                            game_get_square(g, t2_r, t2_c) != EMPTY) continue;

                        if (game_is_neigh(g, t1_r, t1_c, TENT, true) || 
                            game_is_neigh(g, t2_r, t2_c, TENT, true)) continue;

                        game_set_square(g, t1_r, t1_c, TENT);
                        bool touch_each_other = game_is_neigh(g, t2_r, t2_c, TENT, true);
                        game_set_square(g, t1_r, t1_c, EMPTY);

                        if (!touch_each_other) {
                            game_set_square(g, r, c, BIGTREE);
                            game_set_square(g, t1_r, t1_c, TENT);
                            game_set_square(g, t2_r, t2_c, TENT);

                            game_set_expected_nb_tents_row(g, t1_r, game_get_expected_nb_tents_row(g, t1_r) + 1);
                            game_set_expected_nb_tents_col(g, t1_c, game_get_expected_nb_tents_col(g, t1_c) + 1);
                            
                            game_set_expected_nb_tents_row(g, t2_r, game_get_expected_nb_tents_row(g, t2_r) + 1);
                            game_set_expected_nb_tents_col(g, t2_c, game_get_expected_nb_tents_col(g, t2_c) + 1);

                            placed = true;
                        }
                    }
                }
            }
            if (placed) break;
        }
    }

    for (uint i = 0; i < nb_trees; i++) {
        for (int attempt = 0; attempt < 100; attempt++) {
            uint r = rand() % nb_rows;
            uint c = rand() % nb_cols;

            if (game_get_square(g, r, c) != EMPTY) continue;

            int d_idx[4] = {0, 1, 2, 3};
            for (int k = 0; k < 4; k++) {
                int rand_idx = rand() % 4;
                int temp = d_idx[k];
                d_idx[k] = d_idx[rand_idx];
                d_idx[rand_idx] = temp;
            }

            bool placed = false;
            for (int k = 0; k < 4; k++) {
                uint tr, tc;
                if (game_get_adjacent_square(g, r, c, all_dirs[d_idx[k]], &tr, &tc)) {
                    
                    if (game_get_square(g, tr, tc) == EMPTY && 
                        !game_is_neigh(g, tr, tc, TENT, true)) {
                        
                        game_set_square(g, r, c, TREE);
                        game_set_square(g, tr, tc, TENT);

                        game_set_expected_nb_tents_row(g, tr, game_get_expected_nb_tents_row(g, tr) + 1);
                        game_set_expected_nb_tents_col(g, tc, game_get_expected_nb_tents_col(g, tc) + 1);
                        
                        placed = true;
                        break;
                    }
                }
            }
            if (placed) break;
        }
    }

    if (solution) {
        game_fill_grass(g);
    } 
    else {
        for (uint r = 0; r < nb_rows; r++) {
            for (uint c = 0; c < nb_cols; c++) {
                square s = game_get_square(g, r, c);
                if (s == TENT || s == GRASS) {
                    game_set_square(g, r, c, EMPTY);
                }
            }
        }
    }

    return g;
}