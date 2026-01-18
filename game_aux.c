#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_struct.h"

//this function prints the game
void game_print(cgame g)
{
  if (!g) return;

  uint width = g->width;
  uint height = g->height;

  //this loop helps us print the column indices
  printf("   ");
  for (uint j = 0; j < width; j++) {
    printf("%u", j);
  }
  printf("\n");

  //this loop helps us print the top horizontal borders "-"
  printf("   ");
  for (uint j = 0; j < width; j++) {
    printf("-");
  }
  printf("\n");

  //our main loop here will help us print each row 
  for (uint i = 0; i < height; i++) {
    
    //we will print row index and separete with "|"(left border)
    printf("%u |", i);

    for (uint j = 0; j < width; j++) {
      square s = game_get_square(g, i, j);
      char c;

      //we will chose the character according to our square type and print it
      if (s == EMPTY) {
        c = ' ';
      } else if (s == TREE) {
        c = 'x';
      } else if (s == TENT) {
        c = '*';
      } else if (s == GRASS) {
        c = '-';
      } else if (s == BIGTREE) {
        c = '#';
      }

      printf("%c", c);
    }
    //here we will print the right border "|" and then give the expected number of tents of the row
    printf(" | %u\n", game_get_expected_nb_tents_row(g, i));
  }

  //print the bottom borders
  printf("   ");
  for (uint j = 0; j < width; j++) {
    printf("-");
  }
  printf("\n");

  //we will get our expected number of tents for each column
  printf("   ");
  for (uint j = 0; j < width; j++) {
    printf("%u", game_get_expected_nb_tents_col(g, j));
  }
  printf("\n");
}

/* *********************************************************** */

//this function gives us the default game, given in the documentation
game game_default(void)
{
  game g = game_new_empty();

  if (!g) return NULL;

  //we set the tree squares with the given values(the defaults) under the documentation
  game_set_square(g, 0, 0, TREE);
  game_set_square(g, 0, 5, TREE);
  game_set_square(g, 0, 6, TREE);

  game_set_square(g, 2, 6, TREE);

  game_set_square(g, 3, 0, TREE);
  game_set_square(g, 3, 2, TREE);

  game_set_square(g, 4, 4, TREE);

  game_set_square(g, 5, 2, TREE);

  game_set_square(g, 7, 1, TREE);
  game_set_square(g, 7, 2, TREE);
  game_set_square(g, 7, 5, TREE);
  game_set_square(g, 7, 6, TREE);

  //the array for each expected number of tents for each row
  uint row_vals[8] = {1, 2, 1, 2, 2, 0, 1, 3};

  //setting the expected values
  for (uint i = 0; i < 8; i++) {
    game_set_expected_nb_tents_row(g, i, row_vals[i]);
  }

  //the array for each expected number of tents for each column
  uint col_vals[8] = {3, 0, 2, 1, 2, 1, 2, 1};

  //setting the expected values
  for (uint j = 0; j < 8; j++) {
    game_set_expected_nb_tents_col(g, j, col_vals[j]);
  }

  return g;
}

/* *********************************************************** */

game game_default_solution(void)
{
  game g = game_default();
  if (!g) return NULL;

  game_set_square(g, 0, 4, TENT);

  game_set_square(g, 1, 0, TENT);
  game_set_square(g, 1, 6, TENT);

  game_set_square(g, 2, 2, TENT);

  game_set_square(g, 3, 4, TENT);
  game_set_square(g, 3, 6, TENT);

  game_set_square(g, 4, 0, TENT);
  game_set_square(g, 4, 2, TENT);

  game_set_square(g, 6, 5, TENT);

  game_set_square(g, 7, 0, TENT);
  game_set_square(g, 7, 3, TENT);
  game_set_square(g, 7, 7, TENT);

  game_fill_grass(g);

  return g;
}

/* *********************************************************** */

bool game_get_adjacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next)
{
  assert(g != NULL);
  assert(i < g->height);
  assert(j < g->width);
  assert(pi_next != NULL);
  assert(pj_next != NULL);

  int di = 0, dj = 0;

  switch (d) {
    case HERE:
      di = 0;
      dj = 0;
      break;
    case UP:
      di = -1;
      dj = 0;
      break;
    case DOWN:
      di = +1;
      dj = 0;
      break;
    case LEFT:
      di = 0;
      dj = -1;
      break;
    case RIGHT:
      di = 0;
      dj = +1;
      break;
    case UP_LEFT:
      di = -1;
      dj = -1;
      break;
    case UP_RIGHT:
      di = -1;
      dj = +1;
      break;
    case DOWN_LEFT:
      di = +1;
      dj = -1;
      break;
    case DOWN_RIGHT:
      di = +1;
      dj = +1;
      break;
    default:
      return false;
  }

  int ni = (int)i + di;
  int nj = (int)j + dj;

  int h = (int)g->height;
  int w = (int)g->width;

  if (g->wrapping) {
    ni = ((ni % h) + h) % h;
    nj = ((nj % w) + w) % w;
  } else {
    if (ni < 0 || ni >= h || nj < 0 || nj >= w) {
      return false;
    }
  }

  *pi_next = (uint)ni;
  *pj_next = (uint)nj;

  return true;
}

/* *********************************************************** */

bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag)
{
  assert(g != NULL);
  assert(i < g->height);
  assert(j < g->width);

  direction dirs[8] = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

  uint n_dirs = diag ? 8 : 4;

  for (uint k = 0; k < n_dirs; k++) {
    uint ni, nj;
    if (game_get_adjacent_square(g, i, j, dirs[k], &ni, &nj)) {
      if (game_get_square(g, ni, nj) == s) return true;
    }
  }

  return false;
}

/* *********************************************************** */

uint game_nb_neigh(cgame g, uint i, uint j, square s, bool diag)
{
  assert(g != NULL);
  assert(i < g->height);
  assert(j < g->width);

  uint res = 0;
  direction dirs[8] = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};
  uint n_dirs = diag ? 8 : 4;

  int visited[8];
  for (int k = 0; k < 8; k++) visited[k] = -1;
  int visited_count = 0;

  for (uint k = 0; k < n_dirs; k++) {
    uint ni, nj;
    if (game_get_adjacent_square(g, i, j, dirs[k], &ni, &nj)) {
      int linear_index = ni * g->width + nj;

      bool already_seen = false;
      for (int v = 0; v < visited_count; v++) {
        if (visited[v] == linear_index) {
          already_seen = true;
          break;
        }
      }

      if (!already_seen) {
        visited[visited_count++] = linear_index;

        if (game_get_square(g, ni, nj) == s) {
          res++;
        }
      }
    }
  }

  return res;
}
