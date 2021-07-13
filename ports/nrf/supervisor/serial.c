/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
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

#include "py/mphal.h"

#include "supervisor/serial.h"

#if CIRCUITPY_CONSOLE_BLE
#include "ble_uart.h"
#elif CIRCUITPY_CONSOLE_UART
#include <string.h>
#include "nrf_gpio.h"
#include "nrfx_uarte.h"
#endif

#if CIRCUITPY_CONSOLE_BLE

void serial_init(void) {
    ble_uart_init();
}

bool serial_connected(void) {
    return ble_uart_connected();
}

char serial_read(void) {
    return (char)ble_uart_rx_chr();
}

bool serial_bytes_available(void) {
    return ble_uart_stdin_any();
}

void serial_write(const char *text) {
    ble_uart_stdout_tx_str(text);
}

#elif CIRCUITPY_CONSOLE_UART

uint8_t serial_received_char;
nrfx_uarte_t serial_instance = NRFX_UARTE_INSTANCE(0);

void serial_init(void) {
    nrfx_uarte_config_t config = {
        .pseltxd = MICROPY_HW_UART_TX,
        .pselrxd = MICROPY_HW_UART_RX,
        .pselcts = NRF_UARTE_PSEL_DISCONNECTED,
        .pselrts = NRF_UARTE_PSEL_DISCONNECTED,
        .p_context = NULL,
        .hwfc = NRF_UARTE_HWFC_DISABLED,
        .parity = NRF_UARTE_PARITY_EXCLUDED,
        .baudrate = NRF_UARTE_BAUDRATE_115200,
        .interrupt_priority = 7
    };

    nrfx_uarte_uninit(&serial_instance);
    const nrfx_err_t err = nrfx_uarte_init(&serial_instance, &config, NULL);    // no callback for blocking mode

    if (err != NRFX_SUCCESS) {
        NRFX_ASSERT(err);
    }

    // enabled receiving
    nrf_uarte_task_trigger(serial_instance.p_reg, NRF_UARTE_TASK_STARTRX);
}

bool serial_connected(void) {
    return true;
}

char serial_read(void) {
    uint8_t data;
    nrfx_uarte_rx(&serial_instance, &data, 1);
    return data;
}

bool serial_bytes_available(void) {
    return nrf_uarte_event_check(serial_instance.p_reg, NRF_UARTE_EVENT_RXDRDY);
}

void serial_write(const char *text) {
    serial_write_substring(text, strlen(text));
}

void serial_write_substring(const char *text, uint32_t len) {
    if (len == 0) {
        return;
    }

    // EasyDMA can only access SRAM
    uint8_t *tx_buf = (uint8_t *)text;
    if (!nrfx_is_in_ram(text)) {
        tx_buf = (uint8_t *)m_malloc(len, false);
        memcpy(tx_buf, text, len);
    }

    nrfx_uarte_tx(&serial_instance, tx_buf, len);

    if (!nrfx_is_in_ram(text)) {
        m_free(tx_buf);
    }
}

#endif  // CIRCUITPY_CONSOLE_UART
