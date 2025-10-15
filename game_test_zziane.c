#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int test_dummy (){
    printf("NO ERREUR\n ");
    return EXIT_SUCCESS;
} 

int main(int argc, char *argv[])

{
    
    if (strcmp(argv[1], "dummy")==0){
        
        return test_dummy();
    }
    printf("ERREUR\n ");
    return EXIT_FAILURE;
}