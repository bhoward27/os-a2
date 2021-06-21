#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "list.h"
#include "keyboard_receiver.h"

#define MSG_MAX_LEN 512

static pthread_t thread;
static pthread_mutex_t* ok_to_add_local_msg_mutex;
static pthread_cond_t* ok_to_add_local_msg_cond_var; // TODO: Rename.
static List* local_messages = NULL;

void KeyboardReceiver_init(List* local_msgs, pthread_mutex_t* ok_to_access_local_msgs_mutex, 
                            pthread_cond_t* ok_to_access_local_msgs_cond_var) {
    // We assume here that local_msgs has been verified (i.e., not NULL) before being passed.
    printf("Inside KeyboardReceiver_init()\n");
    local_messages = local_msgs;
    ok_to_add_local_msg_mutex = ok_to_access_local_msgs_mutex;
    ok_to_add_local_msg_cond_var = ok_to_access_local_msgs_cond_var;
    pthread_create(&thread, NULL, KeyboardReceiver_thread, NULL);
}

void* KeyboardReceiver_thread() {
    // TODO: Somewhere you should print the local user's name, like "Bob: " and then the cursor is 
    // there for you to type. This may require synchronization.
    // printf("Inside KeyboardReceiver_thread()\n");
    while (1) {
        // TODO: Remember to free() these messages at the appropriate time!
        char* message = (char*) malloc(sizeof(char) * MSG_MAX_LEN);
        if (!message) {
            // TODO: Handle error.
            fprintf(stderr, "Error in KeyboardReceiver_thread(): char* message = malloc() failed.\n");
        }
        char* succeeded = fgets(message, MSG_MAX_LEN, stdin);
        if (succeeded) {
            int result = LIST_FAIL;

            // printf("Approaching KeyboardReceiver's critical section...\n");
            int lock_result = pthread_mutex_lock(ok_to_add_local_msg_mutex);
            {
                // printf("In KeyboardReceiver's critical section\n");
                if (lock_result) { // Not sure if should be in critical section but.. better safe than sorry.
                    // TODO: Handle error.
                    fprintf(
                        stderr, 
                        "Error in KeyboardReceiver_thread(): pthread_mutex_lock = %d.\n", 
                        lock_result
                    );
                    perror("pthread_mutex_lock");
                }
                result = List_append(local_messages, (void*) message);
            }
            int unlock_result = pthread_mutex_unlock(ok_to_add_local_msg_mutex);
            // printf("Exited KeyboardReceiver's critical section\n");
            if (unlock_result) {
                // TODO: Handle error.
                fprintf(
                    stderr, 
                    "Error in KeyboardReceiver_thread(): pthread_mutex_unlock = %d.\n", 
                    unlock_result
                );
                perror("pthread_mutex_unlock");
            }
            
            if (result == LIST_FAIL) {
                // TODO: Handle failure.
                fprintf(stderr, "Error in KeyboardReceiver_thread(): List_append() = LIST_FAIL.\n");
            }

            pthread_mutex_lock(ok_to_add_local_msg_mutex);
            {
                // Signal MessageSender that a new message is available in local_messages.
                pthread_cond_signal(ok_to_add_local_msg_cond_var);
            }
            pthread_mutex_unlock(ok_to_add_local_msg_mutex);

            // Check for "!", which should end the application for both users.
            // The below line merely exits this thread.
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