#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "game_aux.h"
#include "game_struct.h"

game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col) {
  if(squares==NULL || nb_tents_row==NULL || nb_tents_col==NULL){
    return NULL;
  }
  game g=malloc(sizeof(struct game_s));

  if(g==NULL){
    return NULL;
  }

  uint width=DEFAULT_SIZE;
  uint height=DEFAULT_SIZE;

  g->width=width;
  g->height=height;
  
  g->expected_nb_tents_row=malloc(height*sizeof(uint));
  g->expected_nb_tents_col=malloc(width*sizeof(uint));

  for(int i=0; i<height; i++){
    g->expected_nb_tents_row[i]= nb_tents_row[i];
  }
  for(int i=0; i<width; i++){
    g->expected_nb_tents_col[i]= nb_tents_col[i];
  }

  g->grid= malloc(height*sizeof(square*));

  for(int i=0;i<height; i++){
    g->grid[i]= malloc(width*sizeof(square));
  }

  return g;
}

game game_new_empty(void) { return NULL; }

game game_copy(cgame g) {
  game gc = malloc(sizeof(struct game_s));
  assert(gc != NULL);

  gc->width = g->width;
  gc->height = g->height;
  gc->wrapping = g->wrapping;

  gc->expected_nb_tents_row = malloc(g->height * sizeof(uint));
  gc->expected_nb_tents_col = malloc(g->width * sizeof(uint));
  assert(gc->expected_nb_tents_row != NULL);
  assert(gc->expected_nb_tents_col != NULL);

  for (uint i = 0; i < g->height; i++){
        gc->expected_nb_tents_row[i] = g->expected_nb_tents_row[i];
  }
  for (uint j = 0; j < g->width; j++){
        gc->expected_nb_tents_col[j] = g->expected_nb_tents_col[j];
  }

  gc->grid = malloc(g->height * sizeof(square*));
  assert(gc->grid != NULL);

  for (uint i = 0; i < g->height; i++) {
        gc->grid[i] = malloc(g->width * sizeof(square));
        assert(gc->grid[i] != NULL);
        for (uint j = 0; j < g->width; j++) {
            gc->grid[i][j] = g->grid[i][j];
        }
    }

    return gc;
}

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
  assert(g != NULL);
  uint cpt = 0;
  for(uint i=0 ; i < g->height ; i++){
    for(uint j=0 ; j< g->width ; j++){
      if (game_get_square(g,i,j) == TENT) cpt++ ;
    }
  }
  return cpt;
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
  assert(g != NULL);

  uint nb_trees = 0; //compteur de nombre d'arbres
  uint nb_tents =0; //compteur d enombre de tents

  uint h = g->height;
  uint w = g->width;

  // regle 2 pour les colonnes
  for(uint j=0 ; j<w ; j++){
    if (game_get_current_nb_tents_col(g, j) != g->expected_nb_tents_col[j]) return false;
  }

  for(uint i=0 ; i<h ; i++){
    //relge 2 pour les lignes
    if (game_get_current_nb_tents_row(g, i) != g->expected_nb_tents_row[i]) return false;

    for(uint j=0 ; j<w ; j++){
      square s = game_get_square(g,i,j);
      
      if (s == TENT){
        nb_tents ++;
        // regle 1
        if (game_is_neigh(g, i, j, TENT, true)) return false;

        // regle 4
        if (!game_is_neigh(g, i, j, TREE, false)) return false;
      }

      //regle 4
      if (s == TREE && !game_is_neigh(g, i, j, TENT, false)) return false;
      if (s == TREE) nb_trees++ ;
    }
  }
  

  // regle 3
  if (nb_tents != nb_trees) return false;
  
  return true;
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
      if (g->grid[i][j] != TREE && g->grid[i][j] != EMPTY) game_set_square(g, i, j, EMPTY);
    }
  }
}