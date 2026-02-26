/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Damien P. George
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

#include "cycfg_peripherals.h"
#include "uart_stdio.h"

static cy_stc_scb_uart_context_t uart_stdio_context;
static mtb_hal_uart_t uart_stdio_hal_obj;

void uart_stdio_init(void) {
    // Initialise and enable the SCB UART.
    Cy_SCB_UART_Init(CYBSP_DEBUG_UART_HW, &CYBSP_DEBUG_UART_config, &uart_stdio_context);
    Cy_SCB_UART_Enable(CYBSP_DEBUG_UART_HW);

    // Initialise the higher-layer UART object.
    mtb_hal_uart_setup(&uart_stdio_hal_obj, &CYBSP_DEBUG_UART_hal_config, &uart_stdio_context, NULL);
}

int uart_stdio_read_char(void) {
    size_t n = 1;
    uint8_t buf[1];
    mtb_hal_uart_read(&uart_stdio_hal_obj, buf, &n);
    if (n == 1) {
        return buf[0];
    }
    return -1;
}

void uart_stdio_write_strn(const char *str, size_t len) {
    size_t n = len;
    mtb_hal_uart_write(&uart_stdio_hal_obj, (void *)str, &n);
}
