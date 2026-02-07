#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

bool test_game_load(void)
{
  FILE* f= fopen("gameload.txt","r");
  if(f==NULL){
    exit(EXIT_FAILURE);
  }
  fclose(f);
  game g = game_load("gameload.txt");
  assert(g!=NULL);
  if(game_nb_rows(g)!=8)return false;
  if(game_nb_cols(g)!=8)return false;
  if(game_is_wrapping(g)==true)return false;   
  if(g->expected_nb_tents_row[0]!=1)return false;
  if(g->expected_nb_tents_row[5]!=0)return false;
  if(g->expected_nb_tents_row[7]!=3)return false;
  if(g->expected_nb_tents_col[0]!=3)return false;
  if(g->expected_nb_tents_col[5]!=1)return false;
  if(g->expected_nb_tents_col[7]!=1)return false;
  if(g->grid[0][0]!=TREE)return false;
  if(g->grid[0][1]!=EMPTY)return false;
  if(g->grid[3][0]!=TREE)return false;
  if(g->grid[0][2]!=EMPTY)return false;
  return true;
}



int main(int argc, char* argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Erreur\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "game_load") == 0) return test_game_load() ? EXIT_SUCCESS : EXIT_FAILURE;
  if (strcmp(argv[1], "game_save") == 0) return test_game_save() ? EXIT_SUCCESS : EXIT_FAILURE;

  fprintf(stderr, "Erreur\n");
  return EXIT_FAILURE;
}