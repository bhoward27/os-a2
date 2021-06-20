#ifndef _KEYBOARD_RECEIVER_
#define _KEYBOARD_RECEIVER_

void KeyboardReceiver_init(List*);
void* KeyboardReceiver_thread();
void KeyboardReceiver_wait_for_shutdown();

#endif