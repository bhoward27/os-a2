#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "list.h"
#include "utils.h"
#include "message_bundle.h"
#include "printer.h"

static pthread_t thread;
static Message_bundle* incoming;
static char* thread_name = "Printer_thread";

void Printer_init(Message_bundle* incoming_bundle) {
    printf("Inside Printer_init()\n");
    incoming = incoming_bundle;
    int result = pthread_create(&thread, NULL, Printer_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* Printer_thread() {
    print_thread(thread_name);

    pthread_mutex_t* mutex = incoming->mutex;
    pthread_cond_t* cond_var = incoming->cond_var;
    List* incoming_messages = incoming->messages;
    char* remote_name = incoming->remote_name;
    while (1) {
        char* message = NULL;
        // printf("Approaching Printer's critical section...\n");
        int lock_result = pthread_mutex_lock(mutex);
        {
            // printf("In Printers's critical section\n");
            // sleep_msec(10);
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            printf("Printer_thread(): Waiting...\n");
            int wait_result = pthread_cond_wait(cond_var, mutex);
            if (wait_result) err(thread_name, "pthread_cond_wait", wait_result);
            printf("Printer_thread(): Done waiting.\n");
            void* first = List_first(incoming_messages);
            if (first) {
                message = (char*) List_remove(incoming_messages);
            }
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        // printf("Exited Printer's critical section\n");
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }

        if (!message) {
            printf("message is null.\n");
            continue; // No message to print, so check again.
        }
        printf("%s: %s\n\n", remote_name, message);

        if (strncmp("!\n", message, 3) == 0) return NULL;
    }
    return NULL;
}

void Printer_wait_for_shutdown() {
    printf("Inside Printer_wait_for_shutdown()\n");
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}