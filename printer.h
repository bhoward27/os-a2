#ifndef _printer_H_
#define _printer_H_
#include <pthread.h>

void Printer_init();
void Printer_wait_for_shutdown();
void* Printer_thread(void*);
void Printer_signal_next_char();

#endif