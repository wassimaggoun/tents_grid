#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_dummy(void){
    return EXIT_SUCCESS;
}

int main(int argc,char *argv[]){
    if(argc <2)return EXIT_FAILURE;

    if(strcmp(argv[1],"dummy")==0)return test_dummy();

    return EXIT_FAILURE;
}
