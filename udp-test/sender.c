#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define MSG_MAX_LEN 512

int config_socket(struct sockaddr_in* sin, short port) {
    // Address structure.
    memset(sin, 0, sizeof(*sin));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(INADDR_ANY);
    sin->sin_port = htons(port);

    // Create the socket.
    int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Open and bind the socket.
    bind(socket_descriptor, (struct sockaddr*) sin, sizeof(*sin));

    return socket_descriptor;
}

void err(char* parent_function, char* failed_function, int return_val) {
    fprintf(stderr, "Error in %s(): %s = %d.\n", parent_function, failed_function, return_val);
    perror(failed_function);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("s-talk requires four command-line arguments.\n"
            "The format is like so: s-talk [my port number] [remote machine name] [remote port number]\n");
        return -1;
    }

    char* thread_name = "receiver";

    int local_port = (short) atoi(argv[1]);
    char* remote_name = argv[2];
    char* remote_port = argv[3];

    struct sockaddr_in sin;
    int socket_descriptor = config_socket(&sin, local_port);

      // Set up the remote sockaddr.
    struct addrinfo* servinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(remote_name, remote_port, &hints, &servinfo);
    struct sockaddr* sin_remote = servinfo->ai_addr;
    unsigned int sin_len = sizeof(*sin_remote); // Apparently I need sin_len BEFORE calling getaddrinfo... Not sure how to do that.

    while (1) {
        char message[MSG_MAX_LEN]; // TODO: Set to NULL
        char* succeeded = fgets(message, MSG_MAX_LEN, stdin);
        
        if (succeeded) {
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
        else {
            printf("fgets failed.\n");
        }
    }

    return 0;
}