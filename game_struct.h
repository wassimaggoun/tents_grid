#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <stdbool.h>
#include "game.h"  

struct game_s {
    uint width;   
    uint height;  

    square **grid;  

    uint *expected_nb_tents_row;  
    uint *expected_nb_tents_col;  

    bool wrapping;  
};

#endif
























