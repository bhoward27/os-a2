#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include "list.h"
#include "message_bundle.h"
#include "keyboard_receiver.h"
#include "message_sender.h"
#include "message_receiver.h"
#include "printer.h"
#include "utils.h"

// TODO: Delete all unneccessary files.
// TODO: Delete all dead code.
// TODO: Remove all superfluous print statements.

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("s-talk requires four command-line arguments.\n"
            "The format is like so: s-talk [my port number] [remote machine name] [remote port number]\n");
        return -1;
    }
    Message_bundle incoming;
    Message_bundle outgoing;

    incoming.messages = List_create();
    outgoing.messages = List_create();
    if (!(outgoing.messages) || !(incoming.messages)) {
        printf("List_create() failed. Exiting program...\n");
        return -1;
    }
    // TODO: Verify CLI conforms to the format and is reasonable (Do I NEED to verify it?)
    // Expected CLI format:
    //      s-talk [my port number] [remote machine name] [remote port number]
    incoming.local_port = outgoing.local_port = (short) atoi(argv[1]);
    incoming.remote_name = outgoing.remote_name = argv[2];
    incoming.remote_port = outgoing.remote_port = argv[3];
    // TODO: Get local machine name. Not strictly necessary, but would be nice.

    pthread_mutex_t incoming_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t incoming_cond_var = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t outgoing_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t outgoing_cond_var = PTHREAD_COND_INITIALIZER;
    incoming.mutex = &incoming_mutex;
    incoming.cond_var = &incoming_cond_var;
    outgoing.mutex = &outgoing_mutex;
    outgoing.cond_var = &outgoing_cond_var;
        
    struct sockaddr_in sin;
    incoming.socket = outgoing.socket = config_socket(&sin, incoming.local_port);

    int thread_state = 1; // 1 means all threads are running. 0 means at least one thread has finished.

    KeyboardReceiver_init(&outgoing, &thread_state);
    MessageSender_init(&outgoing, &thread_state);
    MessageReceiver_init(&incoming, &thread_state);
    Printer_init(&incoming, &thread_state);

    KeyboardReceiver_wait_for_shutdown();
    MessageSender_wait_for_shutdown();
    MessageReceiver_wait_for_shutdown();
    Printer_wait_for_shutdown();

    /*
        TODO:
            -Destroy mutexes
            -Destroy condition variables
            -Free queued messages with free()
            -Free lists
    */
    
    return 0;
}