#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

#define SIZE DEFAULT_SIZE

int test_dummy(void){
    return EXIT_SUCCESS;
}


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

bool test_game_delete(void){
    game g = game_new_empty();
    if (g == NULL) {
        fprintf(stderr, "Error\n");
        return false;
    }

    game_delete(g);
    return true;
}


bool test_game_set_square(void) {
    game g = game_new_empty();
    if (g == NULL) {
        fprintf(stderr, "Error\n");
        return false;
    }

    game_set_square(g, 0, 0, TREE);
    if (game_get_square(g, 0, 0) != TREE) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    game_set_square(g, 0, 0, TENT);
    if (game_get_square(g, 0, 0) != TENT) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    game_set_square(g, 2, 2, GRASS);
    if (game_get_square(g, 2, 2) != GRASS) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    game_set_square(g, 2, 2, EMPTY);
    if (game_get_square(g, 2, 2) != EMPTY) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    game_set_square(g, SIZE - 1, SIZE - 1, TREE);
    if (game_get_square(g, SIZE - 1, SIZE - 1) != TREE) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    if (game_get_square(g, 4, 5) != EMPTY) {
        fprintf(stderr, "Error\n");
        game_delete(g);
        return false;
    }

    game_delete(g);
    return true;
}


int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "ERROR missing test name argument\n");
        return EXIT_FAILURE;
    }

    if (strcmp("dummy", argv[1]) == 0){
        return test_dummy();
    }

    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;

}