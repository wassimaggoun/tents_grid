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
  game g = game_load("gameload.txt");
  if (!g) return NULL;
  game g1 = game_default();
  bool test1 = game_equal(g, g1);
  game g2 = game_new_empty_ext(9, 8, true);
  bool test2 = !game_equal(g, g2);
  return test1 && test2;
}
