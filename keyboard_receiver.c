#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "list.h"
#include "message_bundle.h"
#include "utils.h"
#include "keyboard_receiver.h"

static pthread_t thread;
static Message_bundle* outgoing = NULL;
static char* thread_name = "KeyboardReceiver_thread";
static int * all_threads_running = NULL;

void KeyboardReceiver_init(Message_bundle* outgoing_bundle, int* thread_state) {
    outgoing = outgoing_bundle;
    all_threads_running = thread_state;
    int result = pthread_create(&thread, NULL, KeyboardReceiver_thread, NULL);
    if (result) err(thread_name, "pthread_create", result);
}

void* KeyboardReceiver_thread() {
    pthread_mutex_t* mutex = outgoing->mutex;
    pthread_cond_t* cond_var = outgoing->cond_var;
    List* outgoing_messages = outgoing->messages;

    while (*all_threads_running) {
        fd_set rfds;
        struct timeval tv;
        int fd = 0;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = tv.tv_usec = 0;
        int retval = select(fd + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1) err(thread_name, "select", retval);
        else if (retval == 0) continue; // No input detected, so check again.
        
        char* message = (char*) malloc(sizeof(char) * MSG_MAX_LEN);
        if (!message) {
            fprintf(stderr, "Error in KeyboardReceiver_thread(): char* message = malloc() failed.\n");
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        char* succeeded = fgets(message, MSG_MAX_LEN, stdin);
        if (succeeded) {
            int result = LIST_FAIL;

            int lock_result = pthread_mutex_lock(mutex);
            {
                if (lock_result) err(thread_name, "pthread_mutex_lock", lock_result);
                result = List_append(outgoing_messages, (void*) message);
                if (result == LIST_FAIL) {
                    fprintf(
                        stderr, 
                        "Error in KeyboardReceiver_thread(): List_append() = LIST_FAIL.\n"
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
          
        }
        else {
            // message was not added to list, so free it now.
            free(message);
        }
    }
    
    int result = LIST_FAIL;

    int lock_result = pthread_mutex_lock(mutex);
    {
        if (lock_result) err(thread_name, "pthread_mutex_lock", lock_result);
        result = List_append(outgoing_messages, NULL);
        if (result == LIST_FAIL) {
            fprintf(
                stderr, 
                "Error in KeyboardReceiver_thread(): List_append() = LIST_FAIL.\n"
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

void KeyboardReceiver_wait_for_shutdown() {
    int result = pthread_join(thread, NULL);
    if (result) err(thread_name, "pthread_join", result);
}