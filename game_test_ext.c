#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_struct.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"


bool test_game_new_empty(void) {
    game g = game_new_empty();
    if (g == NULL) {
        return false;
    }
    for (uint i = 0; i < SIZE; i++) {
        for (uint j = 0; j < SIZE; j++) {
            if (game_get_square(g, i, j) != EMPTY) {
                fprintf(stderr,"Error\n");
                game_delete(g);
                return false;
            }
        }
        if (game_get_expected_nb_tents_row(g, i) != 0 || game_get_expected_nb_tents_col(g, i) != 0) {
            fprintf(stderr,"Error\n");
            game_delete(g);
            return false;
        }
    }
    game_delete(g);
    return true;
}