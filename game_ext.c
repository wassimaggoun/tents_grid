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

  g->width = nb_cols;
  g->height = nb_rows;
  g->wrapping = wrapping;  
}




game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping){
    return;
}





uint game_nb_rows(cgame g){
    return 0;
}





uint game_nb_cols(cgame g){
    return 0;
}




bool game_is_wrapping(cgame g){
    return 0;
}




void game_undo(game g){
    return;
}




void game_redo(game g){
    return;
}