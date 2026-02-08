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

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees, bool solution){
  uint nb_attempts = 0;
  game g=game_new_empty_ext(nb_rows,nb_cols,wrapping);
  uint placed_trees=0;
  uint placed_bigtrees=0;
  direction dirs[4]={UP,DOWN,LEFT,RIGHT};

  while((placed_trees<nb_trees || placed_bigtrees<nb_bigtrees) && nb_attempts < 1000){
    uint i = (((double)rand() * nb_rows) / RAND_MAX);
    uint j = (((double)rand() * nb_cols) / RAND_MAX);
    bool is_tree = true;

    if(game_get_square(g,i,j)!=EMPTY)continue;

    if(placed_trees == nb_trees){
      is_tree = false;
      game_set_square(g,i,j,BIGTREE);
      placed_bigtrees++;
    } else {
      game_set_square(g,i,j,TREE);
      placed_trees++;
    }


    bool placed_tent = false;
    //Case 1: placing a TREE
    if(is_tree){
      for(uint k=0;k<4;k++){
        uint ni,nj;
        square s=game_get_adjacent_square(g,i,j,dirs[k],&ni,&nj);
        if(game_check_move(g,ni,nj,s)){
          placed_tent = true;
          game_set_square(g,ni,nj,TENT);
          break;
        }
      }
      if (!placed_tent){
        game_set_square(g,i,j,EMPTY);
        placed_trees--;
      }
    }

    //Case 2: placing a BIGTREE
    if(!is_tree){
      uint nb_tents_placed = 0;
      uint ni_save;
      uint nj_save;
      for(uint k=0;k<4;k++){
        uint ni,nj;
        square s=game_get_adjacent_square(g,i,j,dirs[k],&ni,&nj);
        if(game_check_move(g,ni,nj,s)){
          game_set_square(g,ni,nj,TENT);
          nb_tents_placed++;
          placed_tent = true;
          ni_save=ni;
          nj_save=nj;
        }
      }
      if (nb_tents_placed != 2){
        if(placed_tent) game_set_square(g,ni_save,nj_save,EMPTY);
        game_set_square(g,i,j,EMPTY);
        placed_bigtrees--;
      }
    }
    nb_attempts ++;
  }

  //Setting the expected number of tents for each row and col
  for(uint row=0; row< nb_rows; row++){
    uint expected_tents = game_get_current_nb_tents_row(g,row);
    game_set_expected_nb_tents_row(g,row,expected_tents);
  }

  for(uint col=0; col< nb_cols; col++){
    uint expected_tents = game_get_current_nb_tents_cols(g,col);
    game_set_expected_nb_tents_col(g,col,expected_tents);
  }

  if(solution){
    game_fill_grass(g);
    return g;
  } else {
    for(uint i = 0; i<nb_rows; i++){
      for(uint j = 0; j<nb_cols; j++){
        if(game_get_square(g,i,j) == TENT){
          game_set_square(g,i,j,EMPTY);
        }
      }
    }

    return g;
  }
}

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