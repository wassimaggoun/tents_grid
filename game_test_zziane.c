#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"

int test_dummy (){
    printf("NO ERREUR\n ");
    return EXIT_SUCCESS;
} 

bool test_check_move(void) {
    game g = game_default();

    bool t1 = game_check_move(g, 2, 3, TENT);      
    bool t2 = !game_check_move(g, 0, 0, TENT);       
    bool t3 = !game_check_move(g, 4, 4, TENT);         
    bool t5 = !game_check_move(g, 5, 2, GRASS); 
    bool t8 = game_check_move(g, 5, 3, GRASS); 
    bool t6 = !game_check_move(g, 100, 100, TENT);
    bool t9 = !game_check_move(g, 1, 1, TREE);

    game_delete(g);

    return t1 && t2 && t3 && t5 && t6 && t8 && t9 ;
}
int main(int argc, char *argv[])

{
    if (argc!=2){
        
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "dummy")==0){
        
        return test_dummy();
    }

    if (strcmp(argv[1], "game_check_move") == 0) {
        if (test_check_move()) {
            printf("TEST game_check_move : OK\n");
            return EXIT_SUCCESS;
        } else {
            printf("TEST game_check_move : ECHEC\n");
            return EXIT_FAILURE;
        }
    }
    printf("ERREUR\n ");
    return EXIT_FAILURE;
}