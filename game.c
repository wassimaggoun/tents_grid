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

bool game_equal(cgame g1, cgame g2) {
  if (g1->width != g2->width || g1->height != g2->height || g1->wrapping != g2->wrapping) return false;

    for (uint i = 0; i < g1->height; i++) {
        if (g1->expected_nb_tents_row[i] != g2->expected_nb_tents_row[i]) return false;
    }

  for (uint i = 0; i < g1->width; i++) {
        if (g1->expected_nb_tents_col[i] != g2->expected_nb_tents_col[i]) return false;
    }

  for (uint i=0 ; i < g1->height ; i++){
    for (uint j=0 ; j < g1->width ; j++){
      if (g1->grid[i][j] != g2->grid[i][j]) return false; 
    }
  }
  return true;
}

void game_delete(game g) {
  if (g == NULL) {
    return;
  }
  free(g);
}

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
    return;
  }
  if (i > g->width) {
    return;
  }
  g->expected_nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
  if (g == NULL) {
    return;
  }
  if (j > g->width) {
    return;
  }
  g->expected_nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i) {
  return g->expected_nb_tents_row[i];
}

uint game_get_expected_nb_tents_col(cgame g, uint j) {
  return g->expected_nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g) {
  uint res = 0;
  for (int i = 0; i < g->height; i++) {
    res += g->expected_nb_tents_row[i];
  }
  return res;
}

uint game_get_current_nb_tents_row(cgame g, uint i) {
  uint result = 0;
  for (uint j=0 ; j < g->width ; j++) {
    if (g->grid[i][j] == TENT) result ++ ;
  }
  return result;
}

uint game_get_current_nb_tents_col(cgame g, uint j) {
  uint result = 0;
  for (uint i=0 ; i < g->height ; i++) {
    if (g->grid[i][j] == TENT) result ++ ;
  }
  return result;
}

uint game_get_current_nb_tents_all(cgame g) {
  (void)g;
  return 0;
}

void game_play_move(game g, uint i, uint j, square s) {
  if(g==NULL || i>= g->height || j >= g->width ){
    return;
  }
  if(s==TREE){
    return;
  }
  if(s!=EMPTY && s!=GRASS && s!= TENT){
    return;
  }
  g->grid[i][j] = s;
}

bool game_check_move(cgame g, uint i, uint j, square s) {
  square mtn = game_get_square(g, i, j);
  if (mtn == TREE) {
    return false;
  }

  if (s == GRASS) {
    return mtn != TENT;
  }

  if (s == TENT) {
    if (mtn == TENT || mtn == TREE) {
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
        game_set_square(g, i, j, GRASS);
      }
    }
  }
}

void game_restart(game g) {
  for (uint i=0 ; i < g->height ; i++){
    for (uint j=0 ; j < g->width ; j++){
      if (g->grid[i][j] != TREE) game_set_square(g, i, j, EMPTY);
    }
  }
}