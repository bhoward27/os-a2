#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "keyboard_receiver.h"
#include "message_sender.h"
#include "message_receiver.h"
#include "printer.h"

int main(int arg_count, char** args) {
    pthread_mutex_t ok_to_access_remote_msgs_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t ok_to_access_local_msgs_mutex = PTHREAD_MUTEX_INITIALIZER;

    List* local_messages = List_create();
    List* remote_messages = List_create();
    if (!local_messages || !remote_messages) {
        printf("List_create() failed. Exiting program...\n");
        return -1;
    }

    // Expected CLI format:
    //      s-talk [my port number] [remote machine name] [remote port number]
    short local_port = (short) atoi(args[1]);
    char* remote_machine_name = args[2];
    char* remote_port = args[3];

    KeyboardReceiver_init(local_messages, &ok_to_access_local_msgs_mutex);
    MessageSender_init(
        local_messages, 
        &ok_to_access_local_msgs_mutex, 
        local_port, remote_machine_name, 
        remote_port
    );
    MessageReceiver_init(remote_messages, &ok_to_access_remote_msgs_mutex, local_port);
    Printer_init(remote_messages, &ok_to_access_remote_msgs_mutex, remote_machine_name);

    KeyboardReceiver_wait_for_shutdown();
    MessageSender_wait_for_shutdown();
    MessageReceiver_wait_for_shutdown();
    Printer_wait_for_shutdown();
    printf("Inside main -- Done.\n");
    return 0;
}