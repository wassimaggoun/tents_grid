#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_struct.h"
#include "game.h"
#include "game_aux.h"


bool test_game_new_empty_ext(void) {
    game g = game_new_empty_ext(9,8,false);
    if (g == NULL) {
        return false;
    }
    for (uint i = 0; i < 9; i++) {
        for (uint j = 0; j < 8; j++) {
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

    //les effets de wrapping a coder

    game_delete(g);
    return true;
}