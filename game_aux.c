#include "game_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"


void game_print(cgame g) {
    (void)g;
}

game game_default(void) {
    return NULL;
}

game game_default_solution(void) {
    return NULL;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d,
                             uint* pi_next, uint* pj_next) {
    (void)g;
    (void)i;
    (void)j;
    (void)d;
    (void)pi_next;
    (void)pj_next;
    return false;
}

bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag) {
    (void)g;
    (void)i;
    (void)j;
    (void)s;
    (void)diag;
    return false;
}


