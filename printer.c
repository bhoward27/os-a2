#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "list.h"
#include "sleep.h"
#include "printer.h"

#define MSG_MAX_LEN 512 // TODO: Don't repeat yourself.

static List* remote_messages = NULL;
static pthread_t thread;
static pthread_mutex_t* ok_to_remove_remote_msg_mutex;
static char* machine_name;

void Printer_init(List* remote_msgs, pthread_mutex_t* ok_to_access_remote_msgs_mutex, 
                                                                            char* mach_name) {
    printf("Inside Printer_init()\n");
    remote_messages = remote_msgs;
    ok_to_remove_remote_msg_mutex = ok_to_access_remote_msgs_mutex;
    machine_name = mach_name;
    pthread_create(&thread, NULL, Printer_thread, NULL);
}

void* Printer_thread() {
    printf("Inside Printer_thread()\n");
    while (1) {
        char* message = NULL;
        printf("Approaching Printer's critical section...\n");
        int lock_result = pthread_mutex_lock(ok_to_remove_remote_msg_mutex);
        {
            printf("In Printers's critical section\n");
            sleep_msec(1000);
            if (lock_result) { // Not sure if should be in critical section but.. better safe than sorry.
                // TODO: Handle error.
                fprintf(
                    stderr, 
                    "Error in Printer_thread(): pthread_mutex_lock = %d.\n", 
                    lock_result
                );
                perror("pthread_mutex_lock");
            }
            void* first = List_first(remote_messages);
            if (first) {
                message = (char*) List_remove(remote_messages);
            }
        }
        int unlock_result = pthread_mutex_unlock(ok_to_remove_remote_msg_mutex);
        printf("Exited Printer's critical section\n");
        if (unlock_result) {
            // TODO: Handle error.
            fprintf(
                stderr, 
                "Error in Printer_thread(): pthread_mutex_unlock = %d.\n", 
                unlock_result
            );
            perror("pthread_mutex_unlock");
        }

        if (!message) continue; // No message to print, so check again.

        printf("%s: %s\n\n", machine_name, message);

        if (strncmp("!\n", message, 3) == 0) return NULL;
    }
    return NULL;
}

void Printer_wait_for_shutdown() {
    printf("Inside Printer_wait_for_shutdown()\n");
    pthread_join(thread, NULL);
}