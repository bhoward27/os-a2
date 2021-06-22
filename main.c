#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "message_bundle.h"
#include "keyboard_receiver.h"
#include "message_sender.h"
#include "message_receiver.h"
#include "printer.h"

int main(int arg_count, char** args) {
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
    incoming.local_port = outgoing.local_port = (short) atoi(args[1]);
    incoming.remote_name = outgoing.remote_name = args[2];
    incoming.remote_port = outgoing.remote_port = args[3];
    // TODO: Get local machine name. Not strictly necessary, but would be nice.

    pthread_mutex_t incoming_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t incoming_cond_var = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t outgoing_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t outgoing_cond_var = PTHREAD_COND_INITIALIZER;
    incoming.mutex = &incoming_mutex;
    incoming.cond_var = &incoming_cond_var;
    outgoing.mutex = &outgoing_mutex;
    outgoing.cond_var = &outgoing_cond_var;

    KeyboardReceiver_init(&outgoing);
    MessageSender_init(&outgoing);
    MessageReceiver_init(&incoming);
    Printer_init(&incoming);

    KeyboardReceiver_wait_for_shutdown();
    MessageSender_wait_for_shutdown();
    MessageReceiver_wait_for_shutdown();
    Printer_wait_for_shutdown();
    printf("Inside main -- Done.\n");
    return 0;
}