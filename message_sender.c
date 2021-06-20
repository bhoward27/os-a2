#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "message_sender.h"

#define MSG_MAX_LEN 512 // TODO: Don't repeat yourself.

static List* local_messages = NULL;
static pthread_t thread;
static pthread_mutex_t* ok_to_remove_local_msg_mutex;
static int socket_descriptor;
static short remote_port;

void MessageSender_init(List* local_msgs, pthread_mutex_t* ok_to_access_local_msgs_mutex, 
                                                                                short rem_port) {
    printf("Inside MessageSender_init()\n");
    local_messages = local_msgs;
    remote_port = rem_port;
    ok_to_remove_local_msg_mutex = ok_to_access_local_msgs_mutex;
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
    sin.sin_port = htons(remote_port);

    // Create the socket.
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    
    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));


    while (1) {
        void* message = NULL;
        // Get the message to be sent.
        pthread_mutex_lock(ok_to_remove_local_msg_mutex);
        {
            void* first = List_first(local_messages);
            if (first) {
                //  Extract the first item.
                message = List_remove(local_messages);
            }
        }
        pthread_mutex_unlock(ok_to_remove_local_msg_mutex);
        if (!message) continue; // No message so check again.

        struct sockaddr_in sin_something; // TODO: Not sure whether to call this remote or local.
        unsigned int sin_len = sizeof(sin_something);
        int result = sendto(
            socket_descriptor, 
            message, 
            MSG_MAX_LEN, 
            0, 
            (struct sockaddr*) &sin_something,
            sin_len
        );
        if (result == -1) {
            // TODO: Handle error.
        }
    }
    return NULL;
}

void MessageSender_wait_for_shutdown() {
    printf("Inside MessageSender_wait_for_shutdown()\n");
    pthread_join(thread, NULL);
}