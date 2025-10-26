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


bool test_game_new(void) {
    square squares[SIZE * SIZE];
    uint tents_row[SIZE];
    uint tents_col[SIZE];
    for (uint i = 0; i < SIZE * SIZE; i++) {
        squares[i] = EMPTY;
    }

    squares[0] = TREE;                     // coin en haut a gauche
    squares[1] = TENT;                     // prochaine colonne
    squares[SIZE + 2] = GRASS;             // prochaine ligne
    squares[( SIZE * SIZE - 1 ) ] = TREE; // coin en bas a droite

    for (uint i = 0; i < SIZE; i++) {
        tents_row[i] = i % 2;    // 0,1,0,1,...
        tents_col[i] = (i + 1) % 2; // 1,0,1,0,...
    }

    game g = game_new(squares, tents_row, tents_col);
    if (g == NULL) {
        fprintf(stderr, "Erreur\n");
        return false;
    }

    for (uint i = 0; i < SIZE; i++) {
        for (uint j = 0; j < SIZE; j++) {
            square expected = squares[i * SIZE + j];  //position dans le tableau en 1 dimension
            if (game_get_square(g, i, j) != expected) {
                fprintf(stderr, "Erreur\n");
                game_delete(g);
                return false;
            }
        }
    }

    for (uint i = 0; i < SIZE; i++) {
        if (game_get_expected_nb_tents_row(g, i) != tents_row[i]) {
            fprintf(stderr, "Erreur\n", i);
            game_delete(g);
            return false;
        }
        if (game_get_expected_nb_tents_col(g, i) != tents_col[i]) {
            fprintf(stderr, "Erreur\n", i);
            game_delete(g);
            return false;
        }
    }

    game_delete(g);
    return true;
}


bool test_game_copy(void) {
    square squares[SIZE * SIZE];
    uint tents_row[SIZE];
    uint tents_col[SIZE];

    // on remplit le tableau squres
    for (uint i = 0; i < SIZE * SIZE; i++) {
        squares[i] = (i % 4 == 0) ? TREE : EMPTY;
    }

    for (uint i = 0; i < SIZE; i++) {
        tents_row[i] = i % 2;         // 0,1,0,1,...
        tents_col[i] = (i + 1) % 2;   // 1,0,1,0,...
    }

    game g1 = game_new(squares, tents_row, tents_col);
    if (g1 == NULL) {
        fprintf(stderr, "Erreur.\n");
        return false;
    }

    // on créer une copie de g1
    game g2 = game_copy(g1);
    if (g2 == NULL) {
        fprintf(stderr, "Erreur\n");
        game_delete(g1);
        return false;
    }

    for (uint i = 0; i < SIZE; i++) {
        if (game_get_expected_nb_tents_row(g1, i) != game_get_expected_nb_tents_row(g2, i) ||
            game_get_expected_nb_tents_col(g1, i) != game_get_expected_nb_tents_col(g2, i)) {
            fprintf(stderr, "Erreur\n");
            game_delete(g1);
            game_delete(g2);
            return false;
        }

        for (uint j = 0; j < SIZE; j++) {
            if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) {
                fprintf(stderr, "Erreur\n");
                game_delete(g1);
                game_delete(g2);
                return false;
            }
        }
    }

    game_set_square(g1, 0, 0, TENT);
    if (game_get_square(g2, 0, 0) == TENT) {
        fprintf(stderr, "Erreur : la copie a été modifiée en même temps que l’original.\n");
        game_delete(g1);
        game_delete(g2);
        return false;
    }

    
    game_delete(g1);
    game_delete(g2);
    return true;
}


bool test_game_won(void) {
    game g = game_new_empty();
    game_set_expected_nb_tents_row(g, 0, 1);
    game_set_expected_nb_tents_col(g, 0, 1);
    game_set_square(g, 0, 0, TREE);
    game_play_move(g, 0, 1, TENT);

    if (!game_won(g)) {
        fprintf(stderr, "Erreur\n");
        game_delete(g);
        return false;
    }

    game g2 = game_new_empty();
    game_set_expected_nb_tents_row(g2, 0, 1);
    game_set_expected_nb_tents_col(g2, 0, 1);
    game_set_square(g2, 0, 0, TREE);

    if (game_won(g2)) {
        fprintf(stderr, "Erreur\n");
        game_delete(g);
        game_delete(g2);
        return false;
    }

    game_delete(g);
    game_delete(g2);

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