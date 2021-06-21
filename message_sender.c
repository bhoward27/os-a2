#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "sleep.h"
#include "list.h"
#include "message_sender.h"

#define MSG_MAX_LEN 512 // TODO: Don't repeat yourself.

static List* local_messages = NULL;
static pthread_t thread;
static pthread_mutex_t* ok_to_remove_local_msg_mutex;
static int socket_descriptor;
static short local_port;
static char* remote_machine_name;
static char* remote_port;

void MessageSender_init(List* local_msgs, pthread_mutex_t* ok_to_access_local_msgs_mutex, 
                            short loc_port, char* rem_name, char* rem_port) {
    printf("Inside MessageSender_init()\n");
    local_messages = local_msgs;
    local_port = loc_port;
    ok_to_remove_local_msg_mutex = ok_to_access_local_msgs_mutex;
    remote_machine_name = rem_name;
    remote_port = rem_port;
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
    sin.sin_port = htons(local_port);

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
    getaddrinfo(remote_machine_name, remote_port, &hints, &servinfo);
    struct sockaddr* sin_remote = servinfo->ai_addr;
    unsigned int sin_len = sizeof(*sin_remote); // Apparently I need sin_len BEFORE calling getaddrinfo... Not sure how to do that.

    while (1) {
        void* message = NULL;
        // Get the message to be sent.
        printf("Approaching MessageSender's critical section...\n");
        int lock_result = pthread_mutex_lock(ok_to_remove_local_msg_mutex);
        {
            printf("In MessageSender's critical section\n");
            sleep_msec(1000);
            if (lock_result) { // Not sure if should be in critical section but.. better safe than sorry.
                // TODO: Handle error.
                fprintf(
                    stderr, 
                    "Error in MessageSender_thread(): pthread_mutex_lock = %d.\n", 
                    lock_result
                );
                perror("pthread_mutex_lock");
            }
            void* first = List_first(local_messages);
            if (first) {
                //  Extract the first item.
                message = List_remove(local_messages);
            }
        }
        int unlock_result = pthread_mutex_unlock(ok_to_remove_local_msg_mutex);
        printf("Exited MessageSender's critical section\n");
        if (unlock_result) {
            // TODO: Handle error.
            fprintf(
                stderr, 
                "Error in MessageSender_thread(): pthread_mutex_unlock = %d.\n", 
                unlock_result
            );
            perror("pthread_mutex_unlock");
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
        if (result == -1) {
            // TODO: Handle error.
            fprintf(stderr, "Error in MessageSender_thread(): result = -1\n");
            perror("sendto");
        }
    }
    return NULL;
}

void MessageSender_wait_for_shutdown() {
    printf("Inside MessageSender_wait_for_shutdown()\n");
    // TODO: Maybe I need a pthread_cancel here? In that case, just call it shutdown (not wait)?

    pthread_join(thread, NULL);
}