#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <stdbool.h>

#include "game.h"
#include "queue.h"

struct game_s {
  uint width;
  uint height;

  square** grid;

  uint* expected_nb_tents_row;
  uint* expected_nb_tents_col;

  bool wrapping;

  queue* undo_stack;  // pile des moves joués
  queue* redo_stack;  // pile des moves annulés
};

typedef struct move_s {
  uint i;
  uint j;
  square before;
  square after;
} move;

#endif