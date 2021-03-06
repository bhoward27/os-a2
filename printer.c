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
static int * all_threads_running = NULL;

void Printer_init(Message_bundle* incoming_bundle, int* thread_state) {
    incoming = incoming_bundle;
    all_threads_running = thread_state;
    int result = pthread_create(&thread, NULL, Printer_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* Printer_thread() {
    pthread_mutex_t* mutex = incoming->mutex;
    pthread_cond_t* cond_var = incoming->cond_var;
    List* incoming_messages = incoming->messages;
    char* remote_name = incoming->remote_name;
    while (*all_threads_running) {
        char* message = NULL;
        int lock_result = pthread_mutex_lock(mutex);
        {
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            int wait_result = pthread_cond_wait(cond_var, mutex);
            if (wait_result) err(thread_name, "pthread_cond_wait", wait_result);
            void* first = List_first(incoming_messages);
            if (first) {
                message = (char*) List_remove(incoming_messages);
            }
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }

        // Can't distinguish between having a null pointer put on the list, and simply
        // having List_remove fail, for whatever reason. So, if List_remove fails, the program
        // will fail.
        // TODO: Improve.
        if (!message) {
            *all_threads_running = 0;
            return NULL;
        }

        printf("%s: %s", remote_name, message);

        if (strncmp("!\n", message, 3) == 0) *all_threads_running = 0;
        free(message);
    }
    return NULL;
}

void Printer_wait_for_shutdown() {
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}