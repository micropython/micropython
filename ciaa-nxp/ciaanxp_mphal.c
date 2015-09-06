/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include <string.h>
#include "ciaanxp_mphal.h"
#include "board.h"

static int interrupt_char;

static volatile mp_uint_t tick_ct = 0;

void SysTick_Handler(void) {
	tick_ct++;
}

void mp_hal_init(void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
	Board_Init();
}

mp_uint_t mp_hal_get_milliseconds(void) {
    return tick_ct;
}

void mp_hal_milli_delay(mp_uint_t ms) {
	uint32_t end = tick_ct + ms;
		while(tick_ct < end)
			__WFI();
}

void mp_hal_set_interrupt_char(int c) {
    interrupt_char = c;
}

int mp_hal_stdin_rx_chr(void) {
   /*
    for (;;) {
        if (uart_rx_any()) {
            return uart_rx_char();
        }
    } */
   for (;;) {
        int r = Board_UARTGetChar();
        if (r!= EOF) {
            return r;
        }
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    for (; len > 0; --len) {
        //uart_tx_char(*str++);
	Board_UARTPutChar(*str++);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    for (; len > 0; --len) {
        if (*str == '\n') {
            //uart_tx_char('\r');
	    Board_UARTPutChar('\r');
        }
        //uart_tx_char(*str++);
	Board_UARTPutChar(*str++);
    }
}
