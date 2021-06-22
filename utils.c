#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

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
