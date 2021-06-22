#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "list.h"
#include "message_bundle.h"
#include "message_receiver.h"

static pthread_t thread;
static Message_bundle* incoming;
static int socket_descriptor;

void MessageReceiver_init(Message_bundle* incoming_bundle) {
    printf("Inside MessageReceiver_init()\n");
    incoming = incoming_bundle;
    pthread_create(&thread, NULL, MessageReceiver_thread, NULL);
}

void* MessageReceiver_thread() {
    printf("Inside MessageReceiver_thread()\n");

    // Set up the socket.
    // Address structure.
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(incoming->local_port);

    // Create the socket.
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

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
            fprintf(stderr, "Error in MessageReceiver_thread(): recvfrom() = -1.\n");
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        int res = LIST_FAIL;
        printf("Approaching MessageReceiver's critical section...\n");
        int lock_result = pthread_mutex_lock(mutex);
        {
            // printf("In MessageReceiver's critical section\n");
            if (lock_result) {
                fprintf(
                    stderr, 
                    "Error in MessageReceiver_thread(): pthread_mutex_lock = %d.\n", 
                    lock_result
                );
                perror("pthread_mutex_lock");
                exit(EXIT_FAILURE);
            }
            // Add new message to end of the list.
            res = List_append(incoming_messages, (void*) message);
            if (res == LIST_FAIL) {
                fprintf(stderr, "Error in MessageReceiver_thread(): List_append() = LIST_FAIL.\n");
            }
            else {
                printf("Trying to signal Printer...\n");
                pthread_cond_signal(cond_var);
                printf("Signalled Printer.\n");
            }
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        // printf("Exited MessageReceiver's critical section\n");
        if (unlock_result) {
            fprintf(
                stderr, 
                "Error in MessageReceiver_thread(): pthread_mutex_unlock = %d.\n", 
                unlock_result
            );
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

void MessageReceiver_wait_for_shutdown() {
    printf("Inside MessageReceiver_wait_for_shutdown()\n");
    pthread_join(thread, NULL);
}