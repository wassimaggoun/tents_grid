#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "game_aux.h"
#include "game_struct.h"

game game_new_ext(uint nb_rows, uint nb_cols, square* squares,uint* nb_tents_row, uint* nb_tents_col, bool wrapping){
    if (!squares || !nb_tents_row || !nb_tents_col) {
        return NULL;
    }

    game g = malloc(sizeof(struct game_s));
    if (!g) {
        return NULL;
    }

    g->width = nb_rows;
    g->height = nb_cols;
    g->wrapping = wrapping;  

    g->grid = malloc(g->height * sizeof(square*));
    if (!g->grid) {
        free(g);
        return NULL;
    }

    for (uint i = 0; i < g->height; i++) {
        g->grid[i] = malloc(g->width * sizeof(square));
        if (!g->grid[i]) {
            for (uint k = 0; k < i; k++) {
                free(g->grid[k]);
            }
            free(g->grid);
            free(g);
            return NULL;
        }
    }

    for (uint i = 0; i < g->height; i++) {
        for (uint j = 0; j < g->width; j++) {
            uint index = i * g->width + j;
            g->grid[i][j] = squares[index];
        }
    }

    g->expected_nb_tents_row = malloc(g->height * sizeof(uint));
    if (!g->expected_nb_tents_row) {
        for (uint i = 0; i < g->height; i++) {
            free(g->grid[i]);
        }
        free(g->grid);
        free(g);
        return NULL;
    }
    for (uint i = 0; i < g->height; i++) {
        g->expected_nb_tents_row[i] = nb_tents_row[i];
    }

    g->expected_nb_tents_col = malloc(g->width * sizeof(uint));
    if (!g->expected_nb_tents_col) {
        free(g->expected_nb_tents_row);
        for (uint i = 0; i < g->height; i++) {
            free(g->grid[i]);
        }
        free(g->grid);
        free(g);
        return NULL;
    }
    for (uint j = 0; j < g->width; j++) {
        g->expected_nb_tents_col[j] = nb_tents_col[j];
    }

    return g;
}




game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping){
    game g = malloc(sizeof(struct game_s));
    if (!g) {
      return NULL;
    }

    g->width = nb_cols;
    g->height = nb_rows;
    g->wrapping = wrapping; 
}





uint game_nb_rows(cgame g){
    if(g==NULL){
        fprintf(stderr,"JEU VIDE");
        exit(EXIT_FAILURE);
    }
        
        
    
    return g->width;
}





uint game_nb_cols(cgame g){
    if(g==NULL){
        fprintf(stderr,"JEU VIDE");
        exit(EXIT_FAILURE);
    }
        
        
    
    return g->height;
    
}




bool game_is_wrapping(cgame g){
    if(g==NULL){
        fprintf(stderr,"JEU VIDE");
        exit(EXIT_FAILURE);
    }

    return g->wrapping;
}




void game_undo(game g){
    return;
}




void game_redo(game g){
    return;
}