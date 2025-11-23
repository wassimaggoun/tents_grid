#include "game_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"
#include <assert.h>


void game_print(cgame g) {
     if (!g) return;

    uint width  = g->width;
    uint height = g->height;

    printf("   ");
    for (uint j = 0; j < width; j++) {
        printf("%u", j);
    }
    printf("\n");

    printf("   ");
    for (uint j = 0; j < width; j++) {
        printf("-");
    }
    printf("\n");

    for (uint i = 0; i < height; i++) {
        printf("%u |", i);
        for (uint j = 0; j < width; j++) {
            square s = game_get_square(g, i, j);
            char c;

            if (s == EMPTY) {
                c = ' ';
            } else if (s == TREE) {
                c = 'x';
            } else if (s == TENT) {
                c = '*';
            } else if (s == GRASS) {
                c = '-';
            }

            printf("%c", c);
        }
        printf(" | %u\n", game_get_expected_nb_tents_row(g, i));
    }

    printf("   ");
    for (uint j = 0; j < width; j++) {
        printf("-");
    }
    printf("\n");

    printf("   ");
    for (uint j = 0; j < width; j++) {
        printf("%u", game_get_expected_nb_tents_col(g, j));
    }
    printf("\n");
}

game game_default(void) {
    uint size = DEFAULT_SIZE;  // 8
    square squares[DEFAULT_SIZE * DEFAULT_SIZE];

    // Initialiser tout à EMPTY
    for (uint i = 0; i < size * size; i++) {
        squares[i] = EMPTY;
    }

    // Petit macro local pour l'indexation ligne-major
    #define IDX(i,j) ((i) * size + (j))

    // Placement des arbres ('x' dans l'affichage de la doc)

    // Ligne 0 : "x    xx"
    squares[IDX(0,0)] = TREE;
    squares[IDX(0,5)] = TREE;
    squares[IDX(0,6)] = TREE;

    // Ligne 1 : "        " (rien)

    // Ligne 2 : "      x "
    squares[IDX(2,6)] = TREE;

    // Ligne 3 : "x  x    "
    squares[IDX(3,0)] = TREE;
    squares[IDX(3,3)] = TREE;

    // Ligne 4 : "     x  "
    squares[IDX(4,5)] = TREE;

    // Ligne 5 : "   x    "
    squares[IDX(5,3)] = TREE;

    // Ligne 6 : "        " (rien)

    // Ligne 7 : " xx  xx"
    squares[IDX(7,1)] = TREE;
    squares[IDX(7,2)] = TREE;
    squares[IDX(7,5)] = TREE;
    squares[IDX(7,6)] = TREE;

    // Nombres de tentes attendues par ligne (colonne de droite dans l'exemple)
    uint nb_tents_row[DEFAULT_SIZE] = {
        1, // ligne 0
        2, // ligne 1
        1, // ligne 2
        2, // ligne 3
        2, // ligne 4
        0, // ligne 5
        1, // ligne 6
        3  // ligne 7
    };

    // Nombres de tentes attendues par colonne (ligne du bas "30212121")
    uint nb_tents_col[DEFAULT_SIZE] = {
        3, // col 0
        0, // col 1
        2, // col 2
        1, // col 3
        2, // col 4
        1, // col 5
        2, // col 6
        1  // col 7
    };

    // Création du jeu à partir de ces données
    return game_new(squares, nb_tents_row, nb_tents_col);
}

game game_default_solution(void) {
    game g = game_default();
    if (!g) return NULL;


    game_set_square(g, 0, 4, TENT);

    game_set_square(g, 1, 0, TENT);
    game_set_square(g, 1, 6, TENT);

    game_set_square(g, 2, 5, TENT);

    game_set_square(g, 3, 2, TENT);
    game_set_square(g, 3, 6, TENT);

    game_set_square(g, 4, 0, TENT);
    game_set_square(g, 4, 4, TENT);


    game_set_square(g, 6, 2, TENT);

    game_set_square(g, 7, 0, TENT);
    game_set_square(g, 7, 3, TENT);
    game_set_square(g, 7, 7, TENT);

    game_fill_grass(g);

    return g;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d,
                             uint* pi_next, uint* pj_next) {

    assert(g != NULL);
    assert(i < g->height);
    assert(j < g->width);
    assert(pi_next != NULL);
    assert(pj_next != NULL);

    int di = 0, dj = 0;

    switch (d) {
        case HERE:        di =  0; dj =  0; break;
        case UP:          di = -1; dj =  0; break;
        case DOWN:        di = +1; dj =  0; break;
        case LEFT:        di =  0; dj = -1; break;
        case RIGHT:       di =  0; dj = +1; break;
        case UP_LEFT:     di = -1; dj = -1; break;
        case UP_RIGHT:    di = -1; dj = +1; break;
        case DOWN_LEFT:   di = +1; dj = -1; break;
        case DOWN_RIGHT:  di = +1; dj = +1; break;
        default: return false;
    }

    int ni = (int)i + di;
    int nj = (int)j + dj;

    if (g->wrapping) {
        if (ni < 0) ni += g->height;
        if (ni >= (int)g->height) ni -= g->height;
        if (nj < 0) nj += g->width;
        if (nj >= (int)g->width) nj -= g->width;
    } else {
        if (ni < 0 || ni >= (int)g->height || nj < 0 || nj >= (int)g->width)
            return false;
    }

    *pi_next = (uint)ni;
    *pj_next = (uint)nj;
    return true;

}


bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag) {
    assert(g != NULL);
    assert(i < g->height);
    assert(j < g->width);

    direction dirs[8] = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

    int n_dirs = diag ? 8 : 4;

    for (uint i = 0; i < n_dirs; i++) {
        uint ni, nj;
        if (game_get_ajacent_square(g, i, j, dirs[i], &ni, &nj)) {
            if (game_get_square(g, ni, nj) == s)
                return true;
        }
    }

    return false;

}

