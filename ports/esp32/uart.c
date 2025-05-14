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


#include "py/runtime.h"
#include "py/mphal.h"
#include "uart.h"

#if MICROPY_HW_ENABLE_UART_REPL

#include <stdio.h>
#include "driver/uart.h" // For uart_get_sclk_freq()
#include "hal/uart_hal.h"
#include "soc/uart_periph.h"

static void uart_irq_handler(void *arg);

// Declaring the HAL structure on the stack saves a tiny amount of static RAM
#define REPL_HAL_DEFN() { .dev = UART_LL_GET_HW(MICROPY_HW_UART_REPL) }

// RXFIFO Full interrupt threshold. Set the same as the ESP-IDF UART driver
#define RXFIFO_FULL_THR (SOC_UART_FIFO_LEN - 8)

// RXFIFO RX timeout threshold. This is in bit periods, so 10==one byte. Same as ESP-IDF UART driver.
#define RXFIFO_RX_TIMEOUT (10)

void uart_stdout_init(void) {
    uart_hal_context_t repl_hal = REPL_HAL_DEFN();
    soc_module_clk_t sclk;
    uint32_t sclk_freq;

    uart_hal_get_sclk(&repl_hal, &sclk); // To restore SCLK after uart_hal_init() resets it
    ESP_ERROR_CHECK(uart_get_sclk_freq(sclk, &sclk_freq));

    uart_hal_init(&repl_hal, MICROPY_HW_UART_REPL); // Sets defaults: 8n1, no flow control
    uart_hal_set_sclk(&repl_hal, sclk);
    uart_hal_set_baudrate(&repl_hal, MICROPY_HW_UART_REPL_BAUD, sclk_freq);
    uart_hal_rxfifo_rst(&repl_hal);
    uart_hal_txfifo_rst(&repl_hal);

    ESP_ERROR_CHECK(
        esp_intr_alloc(uart_periph_signal[MICROPY_HW_UART_REPL].irq,
            ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM,
            uart_irq_handler,
            NULL,
            NULL)
        );

    // Enable RX interrupts
    uart_hal_set_rxfifo_full_thr(&repl_hal, RXFIFO_FULL_THR);
    uart_hal_set_rx_timeout(&repl_hal, RXFIFO_RX_TIMEOUT);
    uart_hal_ena_intr_mask(&repl_hal, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
}

int uart_stdout_tx_strn(const char *str, size_t len) {
    uart_hal_context_t repl_hal = REPL_HAL_DEFN();
    size_t remaining = len;
    uint32_t written = 0;
    // TODO add a timeout
    for (;;) {
        uart_hal_write_txfifo(&repl_hal, (const void *)str, remaining, &written);

        if (written >= remaining) {
            break;
        }
        remaining -= written;
        str += written;
        ulTaskNotifyTake(pdFALSE, 1);
    }
    return len;
}

// all code executed in ISR must be in IRAM, and any const data must be in DRAM
static void IRAM_ATTR uart_irq_handler(void *arg) {
    uint8_t rbuf[SOC_UART_FIFO_LEN];
    int len;
    uart_hal_context_t repl_hal = REPL_HAL_DEFN();

    uart_hal_clr_intsts_mask(&repl_hal, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT | UART_INTR_FRAM_ERR);

    len = uart_hal_get_rxfifo_len(&repl_hal);

    uart_hal_read_rxfifo(&repl_hal, rbuf, &len);

    for (int i = 0; i < len; i++) {
        if (rbuf[i] == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            // this is an inline function so will be in IRAM
            ringbuf_put(&stdin_ringbuf, rbuf[i]);
        }
    }
}

#endif // MICROPY_HW_ENABLE_UART_REPL
