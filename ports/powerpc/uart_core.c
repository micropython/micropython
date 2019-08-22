/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Michael Neuling, IBM Corporation.
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

#include <unistd.h>
#include <stdbool.h>

#include "py/mpconfig.h"
#include "uart_potato.h"
#include "uart_lpc_serial.h"

static int lpc_console;
static int potato_console;

void uart_init_ppc(int lpc) {
    lpc_console = lpc;

    if (!lpc_console) {
        potato_console = 1;

        potato_uart_init();
    } else {
        lpc_uart_init();
    }
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    if (lpc_console) {
        c = lpc_uart_read();
    } else if (potato_console) {
        c = potato_uart_read();
    }
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (lpc_console) {
        int i;
        for (i = 0; i < len; i++) {
            lpc_uart_write(str[i]);
        }
    } else if (potato_console) {
        int i;
        for (i = 0; i < len; i++) {
            potato_uart_write(str[i]);
        }
    }
}
