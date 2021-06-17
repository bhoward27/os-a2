#include "printer.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "printer.h"

static const char* MESSAGE = "Hello world in Sync!";
pthread_t thread;

void* Signaller_thread(void* unused) {
    for (const char* msg = MESSAGE; *msg != '\0'; msg++) {
        sleep_msec(200);
        
        // Signal other thread
        Printer_signal_next_char();
    }
    return NULL;
}

void Signaller_init() {
    pthread_create(&thread, NULL, Signaller_thread, NULL);
}

void Signaller_wait_for_shutdown() {

}