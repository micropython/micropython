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

#include <zephyr.h>
#include <drivers/uart.h>
#include <drivers/console/uart_console.h>
#include <sys/printk.h>
#include "zephyr_getchar.h"

extern int mp_interrupt_char;
void mp_sched_keyboard_interrupt(void);
void mp_hal_signal_event(void);
void mp_hal_wait_sem(struct k_sem *sem, uint32_t timeout_ms);

static struct k_sem uart_sem;
#define UART_BUFSIZE 256
static uint8_t uart_ringbuf[UART_BUFSIZE];
static uint8_t i_get, i_put;

static int console_irq_input_hook(uint8_t ch)
{
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
    //printk("%x\n", ch);
    k_sem_give(&uart_sem);
    k_yield();
    return 1;
}

uint8_t zephyr_getchar(void) {
    mp_hal_wait_sem(&uart_sem, -1);
    k_sem_take(&uart_sem, K_FOREVER);
    unsigned int key = irq_lock();
    uint8_t c = uart_ringbuf[i_get++];
    i_get &= UART_BUFSIZE - 1;
    irq_unlock(key);
    return c;
}

void zephyr_getchar_init(void) {
    k_sem_init(&uart_sem, 0, UINT_MAX);
    uart_console_in_debug_hook_install(console_irq_input_hook);
    // All NULLs because we're interested only in the callback above
    uart_register_input(NULL, NULL, NULL);
}
