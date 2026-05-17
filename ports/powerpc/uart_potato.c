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

/*
 * This is a driver for the potato UART used by the microwatt core.
 * The original potato UART came from here
 *    https://github.com/skordal/potato
 */

#include <unistd.h>
#include <stdbool.h>
#include "py/mpconfig.h"

#define SYSCON_BASE     0xc0000000  /* System control regs */
#define SYS_REG_CLKINFO                 0x20

#define UART_FREQ                           115200
#define POTATO_UART_BASE                    0xc0002000
static uint64_t potato_uart_base;

#define POTATO_CONSOLE_TX                   0x00
#define POTATO_CONSOLE_RX                   0x08
#define POTATO_CONSOLE_STATUS               0x10
#define POTATO_CONSOLE_STATUS_RX_EMPTY      0x01
#define POTATO_CONSOLE_STATUS_TX_EMPTY      0x02
#define POTATO_CONSOLE_STATUS_RX_FULL       0x04
#define POTATO_CONSOLE_STATUS_TX_FULL       0x08
#define POTATO_CONSOLE_CLOCK_DIV            0x18
#define POTATO_CONSOLE_IRQ_EN               0x20

static uint64_t potato_uart_reg_read(int offset) {
    uint64_t addr;
    uint64_t val;

    addr = potato_uart_base + offset;

    val = *(volatile uint64_t *)addr;

    return val;
}

static void potato_uart_reg_write(int offset, uint64_t val) {
    uint64_t addr;

    addr = potato_uart_base + offset;

    *(volatile uint64_t *)addr = val;
}

static int potato_uart_rx_empty(void) {
    uint64_t val;

    val = potato_uart_reg_read(POTATO_CONSOLE_STATUS);

    if (val & POTATO_CONSOLE_STATUS_RX_EMPTY) {
        return 1;
    }

    return 0;
}

static int potato_uart_tx_full(void) {
    uint64_t val;

    val = potato_uart_reg_read(POTATO_CONSOLE_STATUS);

    if (val & POTATO_CONSOLE_STATUS_TX_FULL) {
        return 1;
    }

    return 0;
}

static unsigned long potato_uart_divisor(unsigned long proc_freq, unsigned long uart_freq) {
    return proc_freq / (uart_freq * 16) - 1;
}

void uart_init_ppc(void) {
    uint64_t proc_freq;

    potato_uart_base = POTATO_UART_BASE;
    proc_freq = *(volatile uint64_t *)(SYSCON_BASE + SYS_REG_CLKINFO);
    potato_uart_reg_write(POTATO_CONSOLE_CLOCK_DIV, potato_uart_divisor(proc_freq, UART_FREQ));

}

int mp_hal_stdin_rx_chr(void) {
    uint64_t val;

    while (potato_uart_rx_empty()) {
        ;
    }
    val = potato_uart_reg_read(POTATO_CONSOLE_RX);

    return (char)(val & 0x000000ff);
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    int i;

    for (i = 0; i < len; i++) {
        uint64_t val = str[i];

        while (potato_uart_tx_full()) {
            ;
        }
        potato_uart_reg_write(POTATO_CONSOLE_TX, val);
    }
    return len;
}
