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
void game_save(cgame g, char* filename)
{
  assert(g);
  assert(filename);

  uint height = game_nb_rows(g);
  uint width = game_nb_cols(g);

  FILE* f = fopen(filename, "w");  // Opening the file in writing mode
  if (!f) return;

  fprintf(f, "%u %u %d \n", height, width, game_is_wrapping(g));
  // Writing the expected number of tents in each row.
  for (uint i = 0; i < height; i++) {
    uint x = game_get_current_nb_tents_row(g, i);
    fprintf(f, "%u ", x);
  }
  fprintf(f, "\n");
  // Writing the expected number of tents in each column.
  for (uint j = 0; j < g->width; j++) {
    uint y = game_get_expected_nb_tents_col(g, j);
    fprintf(f, "%u ", y);
  }
  fprintf(f, "\n");

  char c = 'E';
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      switch (s) {
        case EMPTY:
          c = 'E';
          break;
        case TREE:
          c = 'X';
          break;
        case TENT:
          c = 'T';
          break;
        case GRASS:
          c = 'G';
          break;
        case BIGTREE:
          c = 'B';
          break;
        default:
          break;
      }
      fprintf(f, "%c ", c);
    }
    fprintf(f, "\n", c);
  }
  fclose(f);
}

//________________________________________________________________________________________________
game game_load(char* filename)
{
  FILE* f = fopen(filename, "r");
  if (!f) {
    return NULL;
  }
  uint nb_rows, nb_cols;
  int wrapping_int;
  if (fscanf(f, "%u,%u,%d", &nb_rows, &nb_cols, &wrapping_int) != 3) {
    fclose(f);
    return NULL;
  }
  bool wrapping = false;
  if (wrapping_int == 1) {
    wrapping = true;
  }
  uint* nb_tents_rows = malloc(nb_rows * sizeof(uint));
  uint* nb_tents_cols = malloc(nb_cols * sizeof(uint));
  square* squares = malloc(nb_cols * nb_rows * sizeof(square));
  if (!nb_tents_cols || !nb_tents_cols || !squares) {
    goto error;
    fclose(f);
    return NULL;
  }
  for (uint i = 0; i < nb_rows; i++) {
    if (fscanf(f, "&u", &nb_tents_rows[i]) != 1) goto error;
    fclose(f);
    return NULL;
  }
  for (uint i = 0; i < nb_cols; i++) {
    if (fscanf(f, "&u", &nb_tents_cols[i]) != 1) goto error;
    fclose(f);
    return NULL;
  }
  char c;
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (fscanf(f, " %c", &c)) {
        goto error;
      }
      square s = EMPTY;
      switch (c) {
        case 'E':
          s = EMPTY;
          break;
        case 'X':
          s = TREE;
          break;
        case 'T':
          s = TENT;
          break;
        case 'G':
          s = GRASS;
          break;
        case 'B':
          s = BIGTREE;
          break;
        default:
          goto error;
          fclose(f);
          return NULL;
      }
      squares[i * nb_cols + j] = s;
    }
  }
  game g = game_new_ext(nb_rows, nb_cols, squares, nb_tents_rows, nb_tents_cols, wrapping);
  goto error;
  fclose(f);
  return g;
error:
  free(nb_tents_rows);
  free(nb_tents_cols);
  free(squares);
  fclose(f);
  return;
}