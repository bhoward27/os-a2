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
static int socket_descriptor;
static char* thread_name = "MessageSender_thread";

// TODO: Might want to pass a struct in instead, since now all threads require to be initalized with
// List*, pthread_mutex_t*, and pthread_cond_t*
// If change this, should apply to all modules.
void MessageSender_init(Message_bundle* outgoing_bundle) {
    printf("Inside MessageSender_init()\n");
    outgoing = outgoing_bundle;
    pthread_create(&thread, NULL, MessageSender_thread, NULL);
}

void* MessageSender_thread() {
    printf("Inside MessageSender_thread()\n");

    // Set up the socket.
    // Address structure.
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(outgoing->local_port);

    // Create the socket.
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    
    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

    // Set up the remote sockaddr.
    struct addrinfo* servinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(outgoing->remote_name, outgoing->remote_port, &hints, &servinfo);
    struct sockaddr* sin_remote = servinfo->ai_addr;
    unsigned int sin_len = sizeof(*sin_remote); // Apparently I need sin_len BEFORE calling getaddrinfo... Not sure how to do that.

    pthread_mutex_t* mutex = outgoing->mutex;
    pthread_cond_t* cond_var = outgoing->cond_var;
    List* outgoing_messages = outgoing->messages;
    while (1) {
        void* message = NULL;
        // Get the message to be sent.
        // printf("Approaching MessageSender's critical section...\n");
        int lock_result = pthread_mutex_lock(mutex);
        {
            // printf("In MessageSender's critical section\n");
            // sleep_msec(10);
            if (lock_result) {
                err(thread_name, "pthread_mutex_lock", lock_result);
            }
            pthread_cond_wait(cond_var, mutex);
            void* first = List_first(outgoing_messages);
            if (first) message = List_remove(outgoing_messages);
        }
        int unlock_result = pthread_mutex_unlock(mutex);
        // printf("Exited MessageSender's critical section\n");
        if (unlock_result) {
            err(thread_name, "pthread_mutex_unlock", unlock_result);
        }
        
        if (!message) continue; // No message so check again.

        int result = sendto(
            socket_descriptor, 
            message, 
            MSG_MAX_LEN, 
            0, 
            sin_remote,
            sin_len
        );
        printf("Message sent.\n");
        if (result == -1) {
            err(thread_name, "sendto", result);
        }
    }
    return NULL;
}

void MessageSender_wait_for_shutdown() {
    printf("Inside MessageSender_wait_for_shutdown()\n");
    // TODO: Maybe I need a pthread_cancel here? In that case, just call it shutdown (not wait)?

    pthread_join(thread, NULL);
}