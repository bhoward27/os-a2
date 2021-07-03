#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "list.h"
#include "message_bundle.h"
#include "utils.h"
#include "message_receiver.h"

static pthread_t thread;
static Message_bundle* incoming;
static int socket_descriptor;
static char* thread_name = "MessageReceiver_thread";

void MessageReceiver_init(Message_bundle* incoming_bundle, int socket) {
    printf("Inside MessageReceiver_init()\n");
    incoming = incoming_bundle;
    socket_descriptor = socket;
    int result = pthread_create(&thread, NULL, MessageReceiver_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* MessageReceiver_thread() {
    print_thread(thread_name);
    // struct sockaddr_in sin;
    // socket_descriptor = config_socket(&sin, incoming->local_port);

    struct sockaddr_in sin_remote;
    unsigned int sin_len = sizeof(sin_remote);

    pthread_mutex_t* mutex = incoming->mutex;
    pthread_cond_t* cond_var = incoming->cond_var;
    List* incoming_messages = incoming->messages;
    while (1) {
        char message[MSG_MAX_LEN];
        printf("Receiving message...\n");
        int result = recvfrom(
            socket_descriptor,
            message,
            MSG_MAX_LEN,
            0,
            (struct sockaddr*) &sin_remote,
            &sin_len
        );
        printf("Message received.\n");
        if (result == -1) {
            err(thread_name, "recvfrom", result);
        }

        int res = LIST_FAIL;
        printf("Approaching MessageReceiver's critical section...\n");
        int lock_result = pthread_mutex_lock(mutex);
        {
            // printf("In MessageReceiver's critical section\n");
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            // Add new message to end of the list.
            res = List_append(incoming_messages, (void*) message);
            if (res == LIST_FAIL) {
                fprintf(stderr, "Error in MessageReceiver_thread(): List_append() = LIST_FAIL.\n");
            }
            else {
                printf("Trying to signal Printer...\n");
                int signal_result = pthread_cond_signal(cond_var);
                if (signal_result) err(thread_name, "pthread_cond_signal", signal_result);
                printf("Signalled Printer.\n");
            }
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        // printf("Exited MessageReceiver's critical section\n");
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }
    }
    return NULL;
}

void MessageReceiver_wait_for_shutdown() {
    printf("Inside MessageReceiver_wait_for_shutdown()\n");
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}