#include "game_tools.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



game game_load(char* filename){

    FILE* f = fopen(filename, "r");
    if(!f){
        return NULL;
    }
    uint nb_rows, nb_cols;
    int  wrapping_int;
    if (fscanf(f, "%u,%u,%d",&nb_rows, &nb_cols, &wrapping_int)!=3){
        fclose(f);
        return NULL;
        
    }
    bool wrapping=false;
    if (wrapping_int==1){
        wrapping=true;
    }
    uint* nb_tents_rows=malloc(nb_rows*sizeof(uint)); 
    uint* nb_tents_cols=malloc(nb_cols*sizeof(uint));
    square* squares= malloc(nb_cols*nb_rows*sizeof(square));
    if(!nb_tents_cols || ! nb_tents_cols|| !squares){
        free(nb_tents_rows);
        free(nb_tents_cols);
        free(squares);
        fclose(f);
        return NULL;
    }
    for (int i =0; i<nb_rows;i++ ){
        if (fscanf(f,"&u", &nb_tents_rows[i])!=1)
            free(nb_tents_rows);
            free(nb_tents_cols);
            free(squares);
            fclose(f);
            return NULL;
    }
    for (int i =0; i<nb_cols;i++ ){
        if (fscanf(f,"&u", &nb_tents_cols[i])!=1)
            free(nb_tents_rows);
            free(nb_tents_cols);
            free(squares);
            fclose(f);
            return NULL;
        
    }
    char c;
    for (int i =0; i<nb_rows;i++ ){
        for(int j =0 ; j<nb_cols; j++ ){
            if (fscanf(f," %c",&c)){
                free(nb_tents_rows);
                free(nb_tents_cols);
                free(squares);
                fclose(f);
                return NULL;
            }
            square s = EMPTY;
            switch (c)
            {
            case 'E' : s = EMPTY; break;
            case 'X' : s = TREE; break;
            case 'T' : s = TENT; break;
            case 'G' : s = GRASS; break;
            case 'B' : s = BIGTREE; break;
            default:
                free(nb_tents_rows);
                free(nb_tents_cols);
                free(squares);
                fclose(f);
                return NULL;
                
            }
            squares[i*nb_cols+j]=s; 

            
        }
    }
    game g = game_new_ext(nb_rows, nb_cols, squares, nb_tents_rows, nb_tents_cols, wrapping);
    free(nb_tents_rows);
    free(nb_tents_cols);
    free(squares);
    fclose(f);

    return g;



}