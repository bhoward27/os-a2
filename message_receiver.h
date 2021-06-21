#ifndef _MESSAGE_RECEIVER_H
#define _MESSAGE_RECEIVER_H_

#include <pthread.h>
#include "list.h"

void MessageReceiver_init(List*, pthread_mutex_t*, pthread_cond_t*, short);
void* MessageReceiver_thread();
void MessageReceiver_wait_for_shutdown();

#endif