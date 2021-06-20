#ifndef _MESSAGE_SENDER_H_
#define _MESSAGE_SENDER_H_

#include <pthread.h>
#include "list.h"

void MessageSender_init(List*, pthread_mutex_t*, short, char*, char*);
void* MessageSender_thread();
void MessageSender_wait_for_shutdown();


#endif