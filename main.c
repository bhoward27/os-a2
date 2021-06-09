// WHAT FOLLOWS IS FROM Dr. Fraser's video on sockets.

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h> // For strncmp().
#include <unistd.h> // For close().
#include <string.h>

#define MSG_MAX_LEN 1024 // Arbitrary
#define PORT 22110 // Arbitrary

/*
    How to get the arguments from the command-line:
    int main(int arg_count, char** args) {
        printf("Arguments are (%d) are: \n", arg_count);
        for (int i = 0; i < arg_count; i++) {
            printf("Arg %d: %s\n", i, args[i]);
        }
        ...
        return 0;
    }
*/

int main() {
    printf("Brian's Net Listen Test on UDP port %d:\n", PORT);
    printf("Connect using:\n");
    printf("    netcat -u 127.0.0.1 %d\n", PORT);

    // Configure the socket. This method is hardcoding things--there are better ways.
    // Address Structure:
    struct sockaddr_in sin; // _in means internet.
    memset(&sin, 0, sizeof(sin)); // "Wipe out"/clear the data in sin.
    sin.sin_family = AF_INET; // Connection may be from network.
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    // ntonl means host to network long. htons means host to network short.

    // Create the socket.
    int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    // Bind the socket to the port that we specify.
    bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin)); // Opens socket.

    while (1) {
        // Receive data:
        struct sockaddr_in sin_remote;
        unsigned int sin_len = sizeof(sin_remote);
        char message_rx[MSG_MAX_LEN]; // message buffer.
        int bytes_rx = recvfrom(socket_descriptor,
            message_rx, MSG_MAX_LEN, 0,
            (struct sockaddr*) &sin_remote, &sin_len);
        // sin_remote gets filled with sender's IP address.

        // Null terminate the c-string:
        int terminate_idx = (bytes_rx < MSG_MAX_LEN) ? bytes_rx : MSG_MAX_LEN - 1;
        message_rx[terminate_idx] = '\0';
        printf("Message received (%d bytes):\n\n'%s'\n", bytes_rx, message_rx);

        // Extract the value from the message:
        // (Process the message any way your application requires.)
        int inc_me = atoi(message_rx);

        // Create reply:
        // Watch out for buffer overflow!
        // Populate return message.
        char message_tx[MSG_MAX_LEN];
        sprintf(message_tx, "Math: %d + 1 = %d\n", inc_me, inc_me + 1); // Writes the string into message_tx.
        
        // Send reply:
        sin_len = sizeof(sin_remote);
        sendto(socket_descriptor,
            message_tx, strlen(message_tx),
            0,
            (struct sockaddr*) &sin_remote, sin_len);
        // sin_remote is where the message gets sent to.
        // Have client's IP address and port number from receiving the message.
    }

    // Close socket (when done):
    close(socket_descriptor);
    // It may take a few seconds for the OS to finish closing.

    return 0;
}