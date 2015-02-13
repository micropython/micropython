/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include "ets_sys.h"
#include "etshal.h"
#include "uart.h"
#include "esp_mphal.h"

extern void ets_wdt_disable(void);
extern void wdt_feed(void);
extern void ets_delay_us();

void mp_hal_init(void) {
    ets_wdt_disable(); // it's a pain while developing
    uart_init(UART_BIT_RATE_115200, UART_BIT_RATE_115200);
}

void mp_hal_feed_watchdog(void) {
    //ets_wdt_disable(); // it's a pain while developing
    //WRITE_PERI_REG(0x60000914, 0x73);
    //wdt_feed(); // might also work
}

void mp_hal_udelay(uint32_t us) {
    ets_delay_us(us);
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = uart0_rx();
        if (c != -1) {
            return c;
        }
        mp_hal_udelay(1);
        mp_hal_feed_watchdog();
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    while (*str) {
        uart_tx_one_char(UART0, *str++);
    }
}

void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    while (len--) {
        uart_tx_one_char(UART0, *str++);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, uint32_t len) {
    while (len--) {
        if (*str == '\n') {
            uart_tx_one_char(UART0, '\r');
        }
        uart_tx_one_char(UART0, *str++);
    }
}

uint32_t HAL_GetTick(void) {
    // TODO
    return 0;
}

void HAL_Delay(uint32_t Delay) {
    mp_hal_udelay(Delay * 1000);
}

void mp_hal_set_interrupt_char(int c) {
    // TODO
}

uint32_t mp_hal_get_cpu_freq(void) {
    return ets_get_cpu_frequency();
}
