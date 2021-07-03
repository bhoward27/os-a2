#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

void sleep_usec(long usec) {
    struct timespec sleep_time;
    sleep_time.tv_sec = (usec / 1000000);
    sleep_time.tv_nsec = (usec % 1000000) * 1000;
    nanosleep(&sleep_time, NULL);
}

void sleep_msec(long msec) {
    sleep_usec(msec * 1000);
}

void err(char* parent_function, char* failed_function, int return_val) {
    fprintf(stderr, "Error in %s(): %s = %d.\n", parent_function, failed_function, return_val);
    perror(failed_function);
    exit(EXIT_FAILURE);
}

int config_socket(struct sockaddr_in* sin, short port) {
    // Address structure.
    memset(sin, 0, sizeof(*sin));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(INADDR_ANY);
    sin->sin_port = htons(port);

    // Create the socket.
    int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) sin, sizeof(*sin));

    return socket_descriptor;
}

// // Getting printable thread id based off here: https://stackoverflow.com/a/32211287
// // and the code at the end of the man page for syscall.
// void print_thread(char* name) {
//     printf("Inside %s() -- Thread #%ld\n", name, syscall(SYS_gettid));
// }