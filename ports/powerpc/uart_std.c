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
 * This is the LPC serial UART used by POWER9 boxes. This is modelled
 * in the qemu POWER9 machine.
 */

#include <unistd.h>
#include <stdbool.h>

#include "py/mpconfig.h"
#include "io.h"


#define UART_BASE       0xc0002000

/* Taken from skiboot */
#define REG_RBR         0
#define REG_THR         0
#define REG_DLL         0
#define REG_IER         1
#define REG_DLM         1
#define REG_FCR         2
#define REG_IIR         2
#define REG_LCR         3
#define REG_MCR         4
#define REG_LSR         5
#define REG_MSR         6
#define REG_SCR         7

#define LSR_DR          0x01  /* Data ready */
#define LSR_OE          0x02  /* Overrun */
#define LSR_PE          0x04  /* Parity error */
#define LSR_FE          0x08  /* Framing error */
#define LSR_BI          0x10  /* Break */
#define LSR_THRE        0x20  /* Xmit holding register empty */
#define LSR_TEMT        0x40  /* Xmitter empty */
#define LSR_ERR         0x80  /* Error */

#define LCR_DLAB        0x80  /* DLL access */
#define LCR_8BIT        0x03

#define MCR_DTR         0x01
#define MCR_RTS         0x02

#define FCR_EN_FIFO     0x01
#define FCR_CLR_RCVR    0x02
#define FCR_CLR_XMIT    0x04

#define IER_RX          0x01
#define IER_THRE        0x02
#define IER_ALL         0x0f

static uint64_t uart_base;
static unsigned int uart_rshift;

static unsigned long std_uart_reg(unsigned int r) {
    /*
     * microwatt_soc.h has offsets already set for a stride of 4,
     * so we use rshift to shift back to a stride of 1 for qemu
     */
    return uart_base + (r << uart_rshift);
}

static int std_uart_tx_full(void) {
    return !(readb(std_uart_reg(REG_LSR)) & LSR_THRE);
}

static int std_uart_rx_empty(void) {
    return !(readb(std_uart_reg(REG_LSR)) & LSR_DR);
}

static char std_uart_read(void) {
    return readb(std_uart_reg(REG_RBR));
}

static void std_uart_write(char c) {
    writeb(c, std_uart_reg(REG_THR));
}

static unsigned long std_uart_divisor(unsigned long uart_freq, unsigned long bauds) {
    return uart_freq / (bauds * 16);
}

void std_uart_init(unsigned long base, unsigned int reg_shift,
    unsigned int freq, unsigned int bauds) {
    unsigned long div = std_uart_divisor(freq, bauds);

    uart_base = base;
    uart_rshift = reg_shift;

    writeb(LCR_DLAB,           std_uart_reg(REG_LCR));
    writeb(div & 0xff,         std_uart_reg(REG_DLL));
    writeb(div >> 8,           std_uart_reg(REG_DLM));
    writeb(LCR_8BIT,           std_uart_reg(REG_LCR));
    writeb(MCR_DTR | MCR_RTS,  std_uart_reg(REG_MCR));
    writeb(FCR_EN_FIFO |
        FCR_CLR_RCVR |
        FCR_CLR_XMIT,       std_uart_reg(REG_FCR));
}


int std_uart_rx_chr(void) {
    while (std_uart_rx_empty()) {
        ;
    }
    return std_uart_read();
}


void std_uart_tx_strn(const char *str, mp_uint_t len) {
    int i;
    for (i = 0; i < len; i++) {
        while (std_uart_tx_full()) {
            ;
        }
        std_uart_write(str[i]);
    }
}
