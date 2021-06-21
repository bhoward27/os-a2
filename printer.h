#ifndef _PRINTER_H_
#define _PRINTER_H_

#include <pthread.h>
#include "list.h"

void Printer_init(List*, pthread_mutex_t*, char*);
void* Printer_thread();
void Printer_wait_for_shutdown();

#endif