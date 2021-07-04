#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "utils.h"
#include "list.h"
#include "message_bundle.h"
#include "message_sender.h"

static pthread_t thread;
static Message_bundle* outgoing;
static char* thread_name = "MessageSender_thread";
static int * all_threads_running = NULL;

void MessageSender_init(Message_bundle* outgoing_bundle, int* thread_state) {
    outgoing = outgoing_bundle;
    all_threads_running = thread_state;
    int result = pthread_create(&thread, NULL, MessageSender_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* MessageSender_thread() {
    int socket_descriptor = outgoing->socket;

    // Set up the remote sockaddr.
    struct addrinfo* servinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(outgoing->remote_name, outgoing->remote_port, &hints, &servinfo);
    struct sockaddr* sin_remote = servinfo->ai_addr;
    unsigned int sin_len = sizeof(*sin_remote);

    pthread_mutex_t* mutex = outgoing->mutex;
    pthread_cond_t* cond_var = outgoing->cond_var;
    List* outgoing_messages = outgoing->messages;
    while (*all_threads_running) {
        void* message = NULL;
        int lock_result = pthread_mutex_lock(mutex);
        {
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            int wait_result = pthread_cond_wait(cond_var, mutex);
            if (wait_result) err(thread_name, "pthread_cond_wait", wait_result);
            void* first = List_first(outgoing_messages);
            if (first) message = List_remove(outgoing_messages);
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }
        
        if (!message) {
            *all_threads_running = 0;
            freeaddrinfo(servinfo);
            return NULL;
        }

        int result = sendto(
            socket_descriptor, 
            message, 
            strlen((char*) message), 
            0,
            sin_remote,
            sin_len
        );
        if (result == -1) {
            err(thread_name, "sendto", result);
        }
        
        char* msg = (char*) message;
        if (strncmp("!\n", msg, 3) == 0) *all_threads_running = 0;
        free(msg);
    }
    freeaddrinfo(servinfo);
    return NULL;
}

void MessageSender_wait_for_shutdown() {
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}