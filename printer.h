#ifndef _PRINTER_H_
#define _PRINTER_H_

#include <pthread.h>
#include "list.h"
#include "message_bundle.h"

void Printer_init(Message_bundle*);
void* Printer_thread();
void Printer_wait_for_shutdown();

#endif