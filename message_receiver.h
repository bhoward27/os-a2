#ifndef _MESSAGE_RECEIVER_H
#define _MESSAGE_RECEIVER_H_

#include <pthread.h>
#include "list.h"
#include "message_bundle.h"

void MessageReceiver_init(Message_bundle*, int*);
void* MessageReceiver_thread();
void MessageReceiver_wait_for_shutdown();

#endif