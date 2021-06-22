#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "list.h"
#include "message_bundle.h"
#include "keyboard_receiver.h"

static pthread_t thread;
static Message_bundle* outgoing = NULL;

void KeyboardReceiver_init(Message_bundle* outgoing_bundle) {
    printf("Inside KeyboardReceiver_init()\n");
    outgoing = outgoing_bundle;
    pthread_create(&thread, NULL, KeyboardReceiver_thread, NULL);
}

void* KeyboardReceiver_thread() {
    pthread_mutex_t* mutex = outgoing->mutex;
    pthread_cond_t* cond_var = outgoing->cond_var;
    List* outgoing_messages = outgoing->messages;
    // TODO: Somewhere you should print the local user's name, like "Bob: " and then the cursor is 
    // there for you to type. This may require synchronization.
    // printf("Inside KeyboardReceiver_thread()\n");
    while (1) {
        // TODO: Remember to free() these messages at the appropriate time!
        char* message = (char*) malloc(sizeof(char) * MSG_MAX_LEN);
        if (!message) {
            fprintf(stderr, "Error in KeyboardReceiver_thread(): char* message = malloc() failed.\n");
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        // printf("Awaiting input...\n");
        char* succeeded = fgets(message, MSG_MAX_LEN, stdin);
        if (succeeded) {
            int result = LIST_FAIL;

            // printf("Approaching KeyboardReceiver's critical section...\n");
            int lock_result = pthread_mutex_lock(mutex);
            {
                // printf("In KeyboardReceiver's critical section\n");
                if (lock_result) {
                    fprintf(
                        stderr, 
                        "Error in KeyboardReceiver_thread(): pthread_mutex_lock = %d.\n", 
                        lock_result
                    );
                    perror("pthread_mutex_lock");
                    exit(EXIT_FAILURE);
                }
                result = List_append(outgoing_messages, (void*) message);
                if (result == LIST_FAIL) {
                    fprintf(stderr, "Error in KeyboardReceiver_thread(): List_append() = LIST_FAIL.\n");
                }
                else pthread_cond_signal(cond_var);
            }
            int unlock_result = pthread_mutex_unlock(mutex);
            // printf("Exited KeyboardReceiver's critical section\n");
            if (unlock_result) {
                fprintf(
                    stderr, 
                    "Error in KeyboardReceiver_thread(): pthread_mutex_unlock = %d.\n", 
                    unlock_result
                );
                perror("pthread_mutex_unlock");
                exit(EXIT_FAILURE);
            }
          
            if (strncmp("!\n", message, 3) == 0) return NULL;
        }
        else {
            // message was not added to list, so free it now.
            free(message);
        }
    }
    return NULL;
}

void KeyboardReceiver_wait_for_shutdown() {
    printf("Inside KeyboardReceiver_wait_for_shutdown()\n");

    // Waits for thread to finish.
    pthread_join(thread, NULL);
}