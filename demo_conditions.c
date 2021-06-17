#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "printer.h"
#include "signaller.h"

int main(int argc, char** args) {
    // Startup:
    Printer_init();
    Signaller_init();

    // Cleanup:
    Printer_wait_for_shutdown();
    Signaller_wait_for_shutdown();
    printf("\n\nDONE\n");
    return 0;
}