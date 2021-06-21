#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "list.h"
#include "message_receiver.h"

#define MSG_MAX_LEN 512 // TODO: DRY.

// Some static stuff
static List* remote_messages = NULL;
static pthread_t thread;
static pthread_mutex_t* ok_to_add_remote_msg_mutex;
static int socket_descriptor;
static short local_port;

void MessageReceiver_init(List* remote_msgs, pthread_mutex_t* ok_to_access_remote_msgs_mutex,
                                                                                short loc_port) {
    printf("Inside MessageReceiver_init()\n");
    remote_messages = remote_msgs;
    local_port = loc_port;
    ok_to_add_remote_msg_mutex = ok_to_access_remote_msgs_mutex;
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
    sin.sin_port = htons(local_port);

    // Create the socket.
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

    struct sockaddr_in sin_remote;
    unsigned int sin_len = sizeof(sin_remote);
    while (1) {
        char message[MSG_MAX_LEN];
        int result = recvfrom(
            socket_descriptor,
            message,
            MSG_MAX_LEN,
            0,
            (struct sockaddr*) &sin_remote,
            &sin_len
        );
        if (result == -1) {
            // TODO: Handle error.
            fprintf(stderr, "Error in MessageReceiver_thread(): recvfrom() = -1.\n");
        }

        int res = LIST_FAIL;
        printf("Approaching MessageReceiver's critical section...\n");
        int lock_result = pthread_mutex_lock(ok_to_add_remote_msg_mutex);
        {
            printf("In MessageReceiver's critical section\n");
            if (lock_result) { // Not sure if should be in critical section but.. better safe than sorry.
                // TODO: Handle error.
                fprintf(
                    stderr, 
                    "Error in MessageReceiver_thread(): pthread_mutex_lock = %d.\n", 
                    lock_result
                );
                perror("pthread_mutex_lock");
            }
            // Add new message to end of the list.
            res = List_append(remote_messages, (void*) message);
        }
        int unlock_result = pthread_mutex_unlock(ok_to_add_remote_msg_mutex);
        printf("Exited MessageReceiver's critical section\n");
        if (unlock_result) {
            // TODO: Handle error.
            fprintf(
                stderr, 
                "Error in MessageReceiver_thread(): pthread_mutex_unlock = %d.\n", 
                unlock_result
            );
            perror("pthread_mutex_unlock");
        }

        if (res == LIST_FAIL) {
            // TODO: Handle error.
            fprintf(stderr, "Error in MessageReceiver_thread(): List_append() = LIST_FAIL.\n");
        }
    }
    return NULL;
}

void MessageReceiver_wait_for_shutdown() {
    printf("Inside MessageReceiver_wait_for_shutdown()\n");
    pthread_join(thread, NULL);
}