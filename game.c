#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_struct.h"

game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col) {
  return NULL;
}

game game_new_empty(void) { return NULL; }

game game_copy(cgame g) { return NULL; }

bool game_equal(cgame g1, cgame g2) { return false; }

void game_delete(game g) { (void)g; }

void game_set_square(game g, uint i, uint j, square s) {
  (void)g;
  (void)i;
  (void)j;
  (void)s;
}

square game_get_square(cgame g, uint i, uint j) {
  (void)g;
  (void)i;
  (void)j;
  return EMPTY;
}

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
  if (g == NULL) {
    return NULL;
  }
  if (i > g->width) {
    return NULL;
  }
  g->expected_nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
  if (g == NULL) {
    return NULL;
  }
  if (j > g->width) {
    return NULL;
  }
  g->expected_nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i) {
  if (g == NULL) {
    return NULL;
  }
  if (i > g->height){
    return NULL;
  }

  return g->expected_nb_tents_row[i];

}

uint game_get_expected_nb_tents_col(cgame g, uint j) {
  if (g == NULL) {
    return NULL;
  }
  if (j > g->width){
    return NULL;
  }

  return g->expected_nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g) {
    uint res =0;
    for (int i = 0 ; i < g->width ; i++){
        res += g->expected_nb_tents_row[i];
    }
    return res;
}

uint game_get_current_nb_tents_row(cgame g, uint i) {
  (void)g;
  (void)i;
  return 0;
}

uint game_get_current_nb_tents_col(cgame g, uint j) {
  (void)g;
  (void)j;
  return 0;
}

uint game_get_current_nb_tents_all(cgame g) {
  (void)g;
  return 0;
}

void game_play_move(game g, uint i, uint j, square s) {
  (void)g;
  (void)i;
  (void)j;
  (void)s;
}

bool game_check_move(cgame g, uint i, uint j, square s) {
    square mtn = game_get_square(g,i,j);
    if (mtn == TREE  ){
        return false;
    }

    if (s==GRASS){
        return mtn !=TENT;
    }
    
    if (s==TENT){
        if (mtn==TENT || mtn ==TREE){
            return false;
        
        
        }
    }
    return true;
}

bool game_won(cgame g) {
  (void)g;
  return false;
}

void game_fill_grass(game g) {
    for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
        for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_square(g, i, j) == EMPTY) {
                game_set_square(g,i,j,GRASS);
            }
        }
    }
}

void game_restart(game g) {
    (void)g;
}
 

void game_restart(game g) { (void)g; }
