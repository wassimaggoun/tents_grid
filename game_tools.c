#include "game_tools.h"
#include "game.h"
#include "game_aux.h"
#include "game_struct.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void game_save(cgame g, char* filename){
    FILE *f = fopen(filename,"w"); //Opening the file in writing mode
    fprintf(f,"%d %d %d \n", g->height,g->width,g->wrapping);
    //Writing the expected number of tents in each row.
    for(uint i=0; i<g->height; i++){
        uint x = g->expected_nb_tents_row[i];
        fprintf(f,"%d ",x);
    }
    fprintf(f,"\n");
    //Writing the expected number of tents in each column.
    for(uint j=0; j<g->width; j++){
        uint y = g->expected_nb_tents_row[j];
        fprintf(f,"%d ",y);
    }
    fprintf(f,"\n");

    uint size = g->height*g->width ;
    
}