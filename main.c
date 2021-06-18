#include <stdio.h>
#include <stdlib.h>

/*
    How to get the arguments from the command-line:
    int main(int arg_count, char** args) {
        printf("Arguments are (%d) are: \n", arg_count);
        for (int i = 0; i < arg_count; i++) {
            printf("Arg %d: %s\n", i, args[i]);
        }
        ...
        return 0;
    }
*/

int main() {
    printf("Starting...\n");
    printf("Done.\n");

    return 0;
}