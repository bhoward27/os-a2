#ifndef _MESSAGE_BUNDLE_H_
#define _MESSAGE_BUNDLE_H_

#include <pthread.h>
#include "list.h"

#define MSG_MAX_LEN 512

typedef struct {
    List* messages;
    short local_port;
    char* local_name;
    char* remote_port;
    char* remote_name;
    int socket;

    // These variables regulate access to messages.
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_var;
} Message_bundle;

#endif