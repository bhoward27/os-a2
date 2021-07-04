#ifndef _KEYBOARD_RECEIVER_
#define _KEYBOARD_RECEIVER_
#include <pthread.h>
#include "message_bundle.h"

void KeyboardReceiver_init(Message_bundle*, int*);
void* KeyboardReceiver_thread();
void KeyboardReceiver_wait_for_shutdown();

#endif