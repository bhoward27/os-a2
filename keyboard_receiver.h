#ifndef _KEYBOARD_RECEIVER_
#define _KEYBOARD_RECEIVER_
#include <pthread.h>

void KeyboardReceiver_init(List*, pthread_mutex_t*);
void* KeyboardReceiver_thread();
void KeyboardReceiver_wait_for_shutdown();

#endif