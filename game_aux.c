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
}

bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag) {
    (void)g;
    (void)i;
    (void)j;
    (void)s;
    (void)diag;
    return false;
}


