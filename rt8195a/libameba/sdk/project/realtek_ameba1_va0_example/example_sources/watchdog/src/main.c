/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "diag.h"
#include "main.h"

#include "wdt_api.h"

#define RUN_CALLBACK_IF_WATCHDOG_BARKS (0)

void dummy_task() {
    for (int i=0; i<50000000; i++)
        asm(" nop");
}

void small_task() {
    printf("\r\ndoing small task...\r\n");
    dummy_task();
    printf("refresh watchdog\r\n\r\n");
    watchdog_refresh();
}

void big_task() {
    printf("\r\ndoing big task...\r\n");
    for (int i=0; i<10; i++) {
        DiagPrintf("doing dummy task %d\r\n", i);
        dummy_task();
    }
    printf("refresh watchdog\r\n\r\n");
    watchdog_refresh();
}

void my_watchdog_irq_handler(uint32_t id) {
    printf("watchdog barks!!!\r\n");
    watchdog_stop();
}

void main(void) {

    watchdog_init(5000); // setup 5s watchdog

#if RUN_CALLBACK_IF_WATCHDOG_BARKS
    watchdog_irq_init(my_watchdog_irq_handler, 0);
#else
    // system would restart when watchdog barks
#endif

    watchdog_start();

    small_task();
    big_task();

    while(1);
}
