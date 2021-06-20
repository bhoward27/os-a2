#include <stdio.h>
#include <pthread.h>
#include "list.h"
#include "printer.h"

#define MSG_MAX_LEN 512 // TODO: Don't repeat yourself.

static List* remote_messages = NULL;
static pthread_t thread;
static pthread_mutex_t* ok_to_remove_remote_msg_mutex;

void Printer_init(List* remote_msgs, pthread_mutex_t* ok_to_access_remote_msgs_mutex) {
    remote_messages = remote_msgs;
    ok_to_remove_remote_msg_mutex = ok_to_access_remote_msgs_mutex;
    pthread_create(&thread, NULL, Printer_thread, NULL);
}

void* Printer_thread() {
    while (1) {
        char* message = NULL;
        pthread_mutex_lock(ok_to_remove_remote_msg_mutex);
        {
            void* first = List_first(remote_messages);
            if (first) {
                message = (char*) List_remove(remote_messages);
            }
        }
        pthread_mutex_unlock(ok_to_remove_remote_msg_mutex);
        if (!message) continue; // No message to print, so check again.
        // TODO: Show the user's name / machine name instead of generic 'remote user'.
        printf("Remote user: %s\n", message);
    }
    return NULL;
}

void Printer_wait_for_shutdown() {
    pthread_join(thread, NULL);
}