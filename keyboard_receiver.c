#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "list.h"
#include "keyboard_receiver.h"

#define MSG_MAX_LEN 512

static pthread_t thread;
static pthread_mutex_t* ok_to_add_local_msg_mutex;
static List* local_messages = NULL;

void KeyboardReceiver_init(List* local_msgs, pthread_mutex_t* ok_to_access_local_msgs_mutex) {
    // We assume here that local_msgs has been verified (i.e., not NULL) before being passed.
    printf("Inside KeyboardReceiver_init()\n");
    local_messages = local_msgs;
    ok_to_add_local_msg_mutex = ok_to_access_local_msgs_mutex;
    pthread_create(&thread, NULL, KeyboardReceiver_thread, NULL);
}

void* KeyboardReceiver_thread() {
    printf("Inside KeyboardReceiver_thread()\n");
    while (1) {
        // TODO: Remember to free() these messages at the appropriate time!
        char* message = (char*) malloc(sizeof(char) * MSG_MAX_LEN);
        if (!message) {
            // TODO: Handle error.
        }
        char* succeeded = fgets(message, MSG_MAX_LEN, stdin);
        if (succeeded) {
            int result = LIST_FAIL;

            pthread_mutex_lock(ok_to_add_local_msg_mutex);
            {
                result = List_append(local_messages, (void*) message);
            }
            pthread_mutex_unlock(ok_to_add_local_msg_mutex);
            
            if (result == LIST_FAIL) {
                // TODO: Handle failure.
            }

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