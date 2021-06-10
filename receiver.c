#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include "receiver.h"

#define MSG_MAX_LEN 1024 // Arbitrary
#define PORT 22110 // Arbitrary

static pthread_t thread_PID;
static int socket_descriptor;

static void* receive_thread(void*);

void Receiver_init(void) {
    pthread_create(
        &thread_PID,
        NULL, // Attributes
        receive_thread, // Function
        NULL // Arguments
    );
    printf("Brian's Net Listen Test on UDP port %d:\n", PORT);
    printf("Connect using:\n");
    printf("    netcat -u 127.0.0.1 %d\n", PORT);
}

void Receiver_shutdown(void) {
    // Cancel thread.
    pthread_cancel(thread_PID);
    
    // Waits for thread to finish.
    pthread_join(thread_PID, NULL);
}

static void* receive_thread(void* unused) {

    // Configure the socket. This method is hardcoding things--there are better ways.
    // Address Structure:
    struct sockaddr_in sin; // _in means internet.
    memset(&sin, 0, sizeof(sin)); // "Wipe out"/clear the data in sin.
    sin.sin_family = AF_INET; // Connection may be from network.
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    // ntonl means host to network long. htons means host to network short.
    // This has to do with byte-ordering/endian-ness. 
    // Network stuff is Big-endian, so need to convert.

    // Create the socket.
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    // Bind the socket to the port that we specify.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin)); // Opens socket.

    while (1) {
        // Receive data:
        struct sockaddr_in sin_remote;
        unsigned int sin_len = sizeof(sin_remote);
        char message_rx[MSG_MAX_LEN]; // message buffer.
        recvfrom(
            socket_descriptor, 
            message_rx, 
            MSG_MAX_LEN, 
            0, 
            (struct sockaddr*) &sin_remote, 
            &sin_len
        );
        // sin_remote gets filled with sender's IP address.
        // message_rx gets the message from sender written into it.

        printf("Message received: %s\n", message_rx);
    }
        
    printf("Done rx thread!");
    return NULL;
}