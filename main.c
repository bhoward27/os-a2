// WHAT FOLLOWS IS FROM Dr. Fraser's video on sockets and threads.

#include <stdio.h>
#include <stdlib.h>
#include "receiver.h"

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
    Receiver_init();

    // Wait for user input.
    printf("Enter something to kill the receive threads.\n");
    char x;
    int res = scanf("%c", &x);
    if (res < 1) {
        printf("scanf failed with output = %d\n", res);
        return 1;
    }

    Receiver_shutdown();
    printf("Done.\n");

    return 0;
}