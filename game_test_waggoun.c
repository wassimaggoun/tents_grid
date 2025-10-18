#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_dummy(void){
    return EXIT_SUCCESS;
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