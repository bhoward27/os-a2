#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "list.h"
#include "message_bundle.h"
#include "utils.h"
#include "message_receiver.h"

static pthread_t thread;
static Message_bundle* incoming;
static char* thread_name = "MessageReceiver_thread";
static int * all_threads_running = NULL;

void MessageReceiver_init(Message_bundle* incoming_bundle, int* thread_state) {
    incoming = incoming_bundle;
    all_threads_running = thread_state;
    int result = pthread_create(&thread, NULL, MessageReceiver_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* MessageReceiver_thread() {
    int socket_descriptor = incoming->socket;

    struct sockaddr_in sin_remote;
    unsigned int sin_len = sizeof(sin_remote);

    pthread_mutex_t* mutex = incoming->mutex;
    pthread_cond_t* cond_var = incoming->cond_var;
    List* incoming_messages = incoming->messages;
    while (*all_threads_running) {
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(socket_descriptor, &rfds);
        tv.tv_sec = tv.tv_usec = 0;
        int retval = select(socket_descriptor + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1) err(thread_name, "select", retval);
        else if (retval == 0) continue; // No message detected, so check again.

        char* message = (char*) malloc(sizeof(char) * MSG_MAX_LEN);
        if (!message) {
            fprintf(stderr, "Error in MessageReceiver_thread(): char* message = malloc() failed.\n");
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        int result = recvfrom(
            socket_descriptor,
            message,
            MSG_MAX_LEN,
            0,
            (struct sockaddr*) &sin_remote,
            &sin_len
        );
        if (result == -1) {
            err(thread_name, "recvfrom", result);
        }

        int res = LIST_FAIL;
        int lock_result = pthread_mutex_lock(mutex);
        {
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            res = List_append(incoming_messages, (void*) message);
            if (res == LIST_FAIL) {
                fprintf(stderr, "Error in MessageReceiver_thread(): List_append() = LIST_FAIL.\n");
                free(message);
            }
            else {
                int signal_result = pthread_cond_signal(cond_var);
                if (signal_result) err(thread_name, "pthread_cond_signal", signal_result);
            }
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }
    }

    // Add a null pointer to incoming_messages to signal printer.
    int result = LIST_FAIL;

    int lock_result = pthread_mutex_lock(mutex);
    {
        if (lock_result) err(thread_name, "pthread_mutex_lock", lock_result);
        result = List_append(incoming_messages, NULL);
        if (result == LIST_FAIL) {
            fprintf(
                stderr, 
                "Error in MessageReceiver_thread(): List_append() = LIST_FAIL.\n"
            );
        }
        else {
            int signal_result = pthread_cond_signal(cond_var);
            if (signal_result) err(thread_name, "pthread_cond_signal", signal_result);
        }
    }
    int unlock_result = pthread_mutex_unlock(mutex);

    if (unlock_result) {
        err(thread_name, "pthread_mutex_unlock", unlock_result);
    }

    return NULL;
}

void MessageReceiver_wait_for_shutdown() {
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}