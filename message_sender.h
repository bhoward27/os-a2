#ifndef _MESSAGE_SENDER_H_
#define _MESSAGE_SENDER_H_

#include <pthread.h>
#include "list.h"
#include "message_bundle.h"

void MessageSender_init(Message_bundle*, int*);
void* MessageSender_thread();
void MessageSender_wait_for_shutdown();


#endif