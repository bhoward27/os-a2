#include "printer.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"

pthread_t thread_printer;
static const char* MESSAGE = "Hello world in Sync!";

static pthread_cond_t s_sync_ok_to_print_cond_var = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_sync_ok_to_print_mutex = PTHREAD_MUTEX_INITIALIZER;

void* Printer_thread(void* unused) {
    for (const char* msg = MESSAGE; *msg != '\0'; msg++) {
        // Wait until signalled
         pthread_mutex_lock(&s_sync_ok_to_print_mutex);
        {
            pthread_cond_wait(&s_sync_ok_to_print_cond_var, &s_sync_ok_to_print_mutex);
        }
        pthread_mutex_unlock(&s_sync_ok_to_print_mutex);
        printf("%c", *msg);
        fflush(stdout);
    }
    return NULL;
}

void Printer_signal_next_char() {
    pthread_mutex_lock(&s_sync_ok_to_print_mutex);
    {
        pthread_cond_signal(&s_sync_ok_to_print_cond_var);
    }
    pthread_mutex_unlock(&s_sync_ok_to_print_mutex);
}

void Printer_init() {
    pthread_create(&thread_printer, NULL, Printer_thread, NULL);
}

void Printer_wait_for_shutdown() {
    pthread_join(thread_printer, NULL);
}