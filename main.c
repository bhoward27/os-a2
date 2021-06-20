#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "keyboard_receiver.h"

int main(int arg_count, char** args) {
    List* local_messages = List_create();
    if (!local_messages) {
        printf("List_create() failed. Exiting program...\n");
        return -1;
    }
    KeyboardReceiver_init(local_messages);


    KeyboardReceiver_wait_for_shutdown();
    printf("Inside main -- done shutdown\n");
    return 0;
}