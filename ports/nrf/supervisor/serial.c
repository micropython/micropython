/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "mphalport.h"

#if MICROPY_PY_BLE_NUS
#include "ble_uart.h"
#else
#include "nrf_gpio.h"
#endif

#if !defined( NRF52840_XXAA) || ( defined(CFG_HWUART_FOR_SERIAL) && CFG_HWUART_FOR_SERIAL == 1 )

#define INST_NO 0

nrfx_uart_t serial_instance = NRFX_UART_INSTANCE(INST_NO);

void serial_init(void) {
#if MICROPY_PY_BLE_NUS
    ble_uart_init0();
    while (!ble_uart_enabled()) {
        ;
    }
#else
    nrfx_uart_config_t config = NRFX_UART_DEFAULT_CONFIG;
    config.pseltxd = MICROPY_HW_UART_TX;
    config.pselrxd = MICROPY_HW_UART_RX;
    config.hwfc = MICROPY_HW_UART_HWFC ? NRF_UART_HWFC_ENABLED : NRF_UART_HWFC_DISABLED;
#ifdef MICROPY_HW_UART_CTS
    config.pselcts = MICROPY_HW_UART_CTS;
#endif
#ifdef MICROPY_HW_UART_RTS
    config.pselrts = MICROPY_HW_UART_RTS;
#endif

    const nrfx_err_t err = nrfx_uart_init(&serial_instance, &config, NULL);
    if (err == NRFX_SUCCESS)
        NRFX_ASSERT(err);

    nrfx_uart_rx_enable(&serial_instance);
#endif
}

bool serial_connected(void) {
    return true;
}

char serial_read(void) {
    return (char) mp_hal_stdin_rx_chr();
}

bool serial_bytes_available(void) {
    return mp_hal_stdin_any();
}

void serial_write(const char *text) {
    mp_hal_stdout_tx_str(text);
}

#else

#include "tusb.h"

void serial_init(void) {
    // usb is already initialized in board_init()
}


bool serial_connected(void) {
    return tud_cdc_connected();
}

char serial_read(void) {
    return (char) tud_cdc_read_char();
}

bool serial_bytes_available(void) {
    return tud_cdc_available() > 0;
}

void serial_write(const char* text) {
    tud_cdc_write(text, strlen(text));
}

#endif
