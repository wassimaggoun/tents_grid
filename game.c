#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

// creates a new game with default size given the grid and the expected nb of tents in each row/col
game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col) {
  if (!squares || !nb_tents_row || !nb_tents_col) {
    return NULL;
  }

  game g = malloc(sizeof(struct game_s));
  if (!g) {
    return NULL;
  }

  g->width = DEFAULT_SIZE;
  g->height = DEFAULT_SIZE;
  g->wrapping = false; // no wrapping by default

  g->grid = malloc(game_nb_rows(g) * sizeof(square*));
  if (!g->grid) {
    game_delete(g);
    return NULL;
  }

  // allocating the
  for (uint i = 0; i < game_nb_rows(g); i++) {
    g->grid[i] = malloc(game_nb_cols(g) * sizeof(square));
    if (!g->grid[i]) {
      game_delete(g);
      return NULL;
    }
  }

  // filling the grid with the given values in the 2D array "squares"
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      uint index = i * game_nb_cols(g) + j;
      g->grid[i][j] = squares[index];
    }
  }

  // setting the expected nb of tents in each rol according to the array given in parameter
  g->expected_nb_tents_row = malloc(game_nb_rows(g) * sizeof(uint));
  if (!g->expected_nb_tents_row) {
    game_delete(g);
    return NULL;
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    g->expected_nb_tents_row[i] = nb_tents_row[i];
  }

  // setting the expected nb of tents in each col according to the array given in parameter (after
  // allocating the necessary memory)
  g->expected_nb_tents_col = malloc(game_nb_cols(g) * sizeof(uint));
  if (!g->expected_nb_tents_col) {
    game_delete(g);
    return NULL;
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    g->expected_nb_tents_col[j] = nb_tents_col[j];
  }

  g->undo_stack = queue_new();
  g->redo_stack = queue_new();

  if (!g->undo_stack || !g->redo_stack) {
    game_delete(g);
    return NULL;
  }

  return g;
}

/* *********************************************************** */
// creates an new empty game with default size
game game_new_empty(void) {
  uint size = DEFAULT_SIZE;
  // we set all squares to empty
  square squares[DEFAULT_SIZE * DEFAULT_SIZE];
  for (uint i = 0; i < size * size; i++) { // we set all the squares of the game to EMPTY
    squares[i] = EMPTY;
  }

  uint nb_tents_row[DEFAULT_SIZE];
  uint nb_tents_col[DEFAULT_SIZE];

  for (uint i = 0; i < size; i++) { // we set to 0 the excpected number of tents in each row/col
    nb_tents_row[i] = 0;
    nb_tents_col[i] = 0;
  }

  return game_new(squares, nb_tents_row,
                  nb_tents_col); // the function game_new takes care of the rest
}

/* *********************************************************** */
// creates a copy of the game g in a new variable
game game_copy(cgame g) {
  game gc = malloc(sizeof(struct game_s)); // allocating the necessary space for the copy
  assert(gc != NULL);
  // copying the structure fields
  gc->width = game_nb_cols(g);
  gc->height = game_nb_rows(g);
  gc->wrapping = g->wrapping;

  gc->expected_nb_tents_row = malloc(game_nb_rows(g) * sizeof(uint));
  gc->expected_nb_tents_col = malloc(game_nb_cols(g) * sizeof(uint));
  assert(gc->expected_nb_tents_row != NULL);
  assert(gc->expected_nb_tents_col != NULL);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    gc->expected_nb_tents_row[i] = g->expected_nb_tents_row[i];
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    gc->expected_nb_tents_col[j] = g->expected_nb_tents_col[j];
  }
  // allocating space for the grid and filling it with the same values as g
  gc->grid = malloc(game_nb_rows(g) * sizeof(square*));
  assert(gc->grid != NULL);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    gc->grid[i] = malloc(game_nb_cols(g) * sizeof(square));
    assert(gc->grid[i] != NULL);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      gc->grid[i][j] = g->grid[i][j];
    }
  }
  gc->undo_stack = queue_new(); // we don't copy the undo/redo stack as well
  gc->redo_stack = queue_new();
  assert(gc->undo_stack != NULL);
  assert(gc->redo_stack != NULL);

  return gc;
}

/* *********************************************************** */
// checks if two games are equal
bool game_equal(cgame g1, cgame g2) {
  if (!g1 || !g2)
    return false;
  // checking the equality of  all the structure's fields (width, height, expected tents, wrapping)
  if (g1->width != g2->width || g1->height != g2->height || g1->wrapping != g2->wrapping)
    return false;

  if ((g1->expected_nb_tents_row == NULL) != (g2->expected_nb_tents_row == NULL))
    return false;

  if (g1->expected_nb_tents_row) {
    for (uint i = 0; i < g1->height; i++) {
      if (g1->expected_nb_tents_row[i] != g2->expected_nb_tents_row[i])
        return false;
    }
  }

  if ((g1->expected_nb_tents_col == NULL) != (g2->expected_nb_tents_col == NULL))
    return false;

  if (g1->expected_nb_tents_col) {
    for (uint j = 0; j < g1->width; j++) {
      if (g1->expected_nb_tents_col[j] != g2->expected_nb_tents_col[j])
        return false;
    }
  }
  // checking the equality square by square
  for (uint i = 0; i < g1->height; i++) {
    for (uint j = 0; j < g1->width; j++) {
      if (game_get_square(g1, i, j) != game_get_square(g2, i, j))
        return false;
    }
  }

  return true;
}

/* *********************************************************** */
// deletes the game and frees the alllocated memory (frees all the arrays of the game dynamically)
void game_delete(game g) {
  if (!g)
    return;

  if (g->grid) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      free(g->grid[i]);
    }
    free(g->grid);
  }

  free(g->expected_nb_tents_row);
  free(g->expected_nb_tents_col);

  if (g->undo_stack)
    queue_free_full(g->undo_stack, free);
  if (g->redo_stack)
    queue_free_full(g->redo_stack, free);

  free(g);
}

/* *********************************************************** */
// sets the square i,j to the value s
void game_set_square(game g, uint i, uint j, square s) {
  if (!g)
    return;
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g))
    return;
  if (s != EMPTY && s != TREE && s != TENT && s != GRASS && s != BIGTREE)
    return;

  g->grid[i][j] = s;
}

/* *********************************************************** */
// returns the value of the square in i,j
square game_get_square(cgame g, uint i, uint j) {
  if (!g)
    return EMPTY;
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g))
    return EMPTY;

  return g->grid[i][j];
}

/* *********************************************************** */
// sets how many tents arex excpected in row i (and stores it in the array)

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
  if (g == NULL) {
    return;
  }
  if (i >= game_nb_rows(g)) {
    return;
  }
  g->expected_nb_tents_row[i] = nb_tents;
}

/* *********************************************************** */
// sets how many tents arex excpected in col j (and stores it in the array)
void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
  if (g == NULL) {
    return;
  }
  if (j >= game_nb_cols(g)) {
    return;
  }
  g->expected_nb_tents_col[j] = nb_tents;
}

/* *********************************************************** */
// returns the excpected number of tents in the row i
uint game_get_expected_nb_tents_row(cgame g, uint i) {
  assert(g);
  assert(g->expected_nb_tents_row);
  assert(i < game_nb_rows(g));

  return g->expected_nb_tents_row[i];
}

/* *********************************************************** */
// returns the expected number of tents in the column j
uint game_get_expected_nb_tents_col(cgame g, uint j) {
  assert(g);
  assert(g->expected_nb_tents_col);
  assert(j < game_nb_cols(g));

  return g->expected_nb_tents_col[j];
}

/* *********************************************************** */
// returns the expected number of tents in the game
uint game_get_expected_nb_tents_all(cgame g) {
  assert(g);
  assert(g->expected_nb_tents_row);
  uint res = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    res += game_get_expected_nb_tents_row(g, i);
  }
  return res;
}

/* *********************************************************** */
// counts the current number of tents in the row i
uint game_get_current_nb_tents_row(cgame g, uint i) {
  assert(g);
  assert(i < game_nb_rows(g));

  uint res = 0;
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == TENT) {
      res++;
    }
  }
  return res;
}

/* *********************************************************** */
// counts the current number of cells in the column j
uint game_get_current_nb_tents_col(cgame g, uint j) {
  assert(g);
  assert(j < game_nb_cols(g));

  uint res = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_square(g, i, j) == TENT) {
      res++;
    }
  }
  return res;
}

/* *********************************************************** */
// counts the current number of tents in the game
uint game_get_current_nb_tents_all(cgame g) {
  assert(g != NULL);
  uint cpt = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TENT)
        cpt++;
    }
  }
  return cpt;
}

/* *********************************************************** */
// Sets a the square i,j with the value s
void game_play_move(game g, uint i, uint j, square s) {
  assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));

  if (game_get_square(g,i,j) == TREE || game_get_square(g,i,j) == BIGTREE) {
    return;
  }
  if (s != EMPTY && s != GRASS && s != TENT) {
    return;
  }

  move* m = malloc(sizeof(move));
  if (!m) {
    fprintf(stderr, "Erreur d'allocation de mémoire.");
    exit(EXIT_FAILURE);
  }
  m->i = i;
  m->j = j;
  m->before = g->grid[i][j];
  m->after = s;

  g->grid[i][j] = s;
  queue_push_tail(g->undo_stack, m);     // adding the move to the undo stack
  queue_clear_full(g->redo_stack, free); // we empty the redo stack after playing a new move
}

/* *********************************************************** */
// check if the move s in the square i,j is legal
bool game_check_move(cgame g, uint i, uint j, square s) {
  if (!g)
    return false;

  if (i >= g->height || j >= g->width) {
    return false;
  }

  if (s != EMPTY && s != GRASS && s != TENT) { // we can only add a tent and add/remove grass
    return false;
  }

  if(s == TENT){
    if(game_is_neigh(g,i,j,TENT,true)) return false;
  }

  square current = game_get_square(g,i,j);
  if (current == TREE || current == BIGTREE) { // We cannot play a move on a tree
    return false;
  }

  if (s == TREE || s == BIGTREE) { // We cannot place a tree
    return false;
  }

  return true;
}

/* *********************************************************** */
// Checks if the game is won in the actual state according to the 4 rules of the game
bool game_won(cgame g) {
  if (!g)
    return false;

  uint total_tents = 0;
  uint required_tents = 0; // at least 1 tent required for TREE and 2 tents for BIGTREE (nb_tress + 2*nb_bigtrees)

  // RULE 2: chekcing if nb_tents = expected_nb_tents in each row/colomn
  for (uint i = 0; i < game_nb_rows(g); i++)
    if (game_get_current_nb_tents_row(g, i) != game_get_expected_nb_tents_row(g, i))
      return false;

  for (uint j = 0; j < game_nb_cols(g); j++)
    if (game_get_current_nb_tents_col(g, j) != game_get_expected_nb_tents_col(g, j))
      return false;

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);

      if (s == TENT) {
        total_tents++;

        // RULE 1: checking if there are adjacent tents
        if (game_is_neigh(g, i, j, TENT, true))
          return false;

        // RULE 4: checking if each tent is orthogonally adjacent to at least one tree
        uint trees = game_nb_neigh(g, i, j, TREE, false) + game_nb_neigh(g, i, j, BIGTREE, false);

        if (trees == 0)
          return false;
      }

      if (s == TREE) {
        required_tents++; // at least 1 tent for TREE

        if (!game_is_neigh(g, i, j, TENT, false))
          return false;
      }

      if (s == BIGTREE) {
        if (game_nb_neigh(g, i, j, TENT, false) != 2)
          return false;
        required_tents += 2; // 2 tents for BIGTREE
      }
    }
  }

  // RULE 3: the total number of tents must match the corresponding nummber of trees
  if (total_tents != required_tents)
    return false;

  return true;
}

/* *********************************************************** */
// fills all the empty squares with grass
void game_fill_grass(game g) {
  assert(g);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        game_set_square(g, i, j, GRASS);
      }
    }
  }
}

/* *********************************************************** */
// Restarts the game
void game_restart(game g) {
  if (!g)
    return;
  // we free the undo/redo stacks
  if (g->undo_stack)
    queue_clear_full(g->undo_stack, free);
  if (g->redo_stack)
    queue_clear_full(g->redo_stack, free);

  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      square s = g->grid[i][j];

      if (s != TREE && s != BIGTREE) { // We empty every square except trees
        g->grid[i][j] = EMPTY;
      }
    }
  }
}