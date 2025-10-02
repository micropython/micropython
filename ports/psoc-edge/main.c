#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #include "py/builtin.h"
// #include "py/compile.h"
// #include "py/runtime.h"
// #include "py/repl.h"
// #include "py/gc.h"
// #include "py/mperrno.h"
// #include "shared/runtime/pyexec.h"

#include "cybsp.h"
#include "retarget_io_init.h"

int main(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();

    /* Board initialization failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result) {
        handle_app_error();
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io middleware */
    init_retarget_io();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen. */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
        "Hello from PSOC Edge 84 AI"
        "****************** \r\n\n");

    printf("We are gradually getting this to compile all the Micropythons sources!!!\r\n\n");

    printf("We should commit the progress now!! \r\n\n");

    for (;;)
    {

    }
}
