/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "driver/uart.h"
#include "soc/uart_periph.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "uart.h"

STATIC void uart_irq_handler(void *arg);

void uart_stdout_init(void) {
    uart_config_t uartcfg = {
        .baud_rate = MICROPY_HW_UART_REPL_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0
    };
    uart_param_config(MICROPY_HW_UART_REPL, &uartcfg);

    const uint32_t rxbuf = 129; // IDF requires > 128 min
    const uint32_t txbuf = 0;

    uart_driver_install(MICROPY_HW_UART_REPL, rxbuf, txbuf, 0, NULL, 0);

    uart_isr_handle_t handle;
    uart_isr_free(MICROPY_HW_UART_REPL);
    uart_isr_register(MICROPY_HW_UART_REPL, uart_irq_handler, NULL, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, &handle);
    uart_enable_rx_intr(MICROPY_HW_UART_REPL);
}

int uart_stdout_tx_strn(const char *str, size_t len) {
    size_t remaining = len;
    // TODO add a timeout
    for (;;) {
        int ret = uart_tx_chars(MICROPY_HW_UART_REPL, str, remaining);
        if (ret == -1) {
            return -1;
        }
        remaining -= ret;
        if (remaining <= 0) {
            break;
        }
        str += ret;
        ulTaskNotifyTake(pdFALSE, 1);
    }
    return len - remaining;
}

// all code executed in ISR must be in IRAM, and any const data must be in DRAM
STATIC void IRAM_ATTR uart_irq_handler(void *arg) {
    volatile uart_dev_t *uart = &UART0;
    #if CONFIG_IDF_TARGET_ESP32S3
    uart->int_clr.rxfifo_full_int_clr = 1;
    uart->int_clr.rxfifo_tout_int_clr = 1;
    #else
    uart->int_clr.rxfifo_full = 1;
    uart->int_clr.rxfifo_tout = 1;
    uart->int_clr.frm_err = 1;
    #endif
    while (uart->status.rxfifo_cnt) {
        #if CONFIG_IDF_TARGET_ESP32
        uint8_t c = uart->fifo.rw_byte;
        #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        uint8_t c = READ_PERI_REG(UART_FIFO_AHB_REG(0)); // UART0
        #endif
        if (c == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            // this is an inline function so will be in IRAM
            ringbuf_put(&stdin_ringbuf, c);
        }
    }
}
