/*
 * Copyright (c) 2016 Linaro
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/console/uart_console.h>
#include <zephyr/sys/printk.h>
#include "zephyr_getchar.h"

extern int mp_interrupt_char;
void mp_sched_keyboard_interrupt(void);
void mp_hal_signal_event(void);

#define UART_BUFSIZE (512)
static uint8_t uart_ringbuf[UART_BUFSIZE];
static uint16_t i_get, i_put;

static int console_irq_input_hook(uint8_t ch) {
    int i_next = (i_put + 1) & (UART_BUFSIZE - 1);
    if (i_next == i_get) {
        printk("UART buffer overflow - char dropped\n");
        return 1;
    }
    if (ch == mp_interrupt_char) {
        mp_hal_signal_event();
        mp_sched_keyboard_interrupt();
        return 1;
    } else {
        uart_ringbuf[i_put] = ch;
        i_put = i_next;
    }
    return 1;
}

// Returns true if a char is available for reading.
int zephyr_getchar_check(void) {
    return i_get != i_put;
}

int zephyr_getchar(void) {
    if (i_get != i_put) {
        unsigned int key = irq_lock();
        int c = (int)uart_ringbuf[i_get++];
        i_get &= UART_BUFSIZE - 1;
        irq_unlock(key);
        return c;
    }
    return -1;
}

void zephyr_getchar_init(void) {
    uart_console_in_debug_hook_install(console_irq_input_hook);
    // All NULLs because we're interested only in the callback above
    uart_register_input(NULL, NULL, NULL);
}
