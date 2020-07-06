/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#include "pin_static_af.h"
#include "nimble/ble.h"
#include "extmod/nimble/hal/hal_uart.h"
#include "extmod/modbluetooth_hci.h"
#include "extmod/nimble/nimble/nimble_hci_uart.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

static hal_uart_tx_cb_t hal_uart_tx_cb;
static void *hal_uart_tx_arg;
static hal_uart_rx_cb_t hal_uart_rx_cb;
static void *hal_uart_rx_arg;

int hal_uart_init_cbs(uint32_t port, hal_uart_tx_cb_t tx_cb, void *tx_arg, hal_uart_rx_cb_t rx_cb, void *rx_arg) {
    hal_uart_tx_cb = tx_cb;
    hal_uart_tx_arg = tx_arg;
    hal_uart_rx_cb = rx_cb;
    hal_uart_rx_arg = rx_arg;
    return 0; // success
}

int hal_uart_config(uint32_t port, uint32_t baudrate, uint32_t bits, uint32_t stop, uint32_t parity, uint32_t flow) {
    mp_bluetooth_hci_uart_init(port);
    mp_bluetooth_hci_uart_set_baudrate(baudrate);
    return mp_bluetooth_hci_uart_activate();
}

void hal_uart_start_tx(uint32_t port) {
    size_t len = 0;
    for (;;) {
        int data = hal_uart_tx_cb(hal_uart_tx_arg);
        if (data == -1) {
            break;
        }
        mp_bluetooth_hci_cmd_buf[len++] = data;
    }

    #if 0
    printf("[% 8d] BTUTX: %02x", mp_hal_ticks_ms(), hci_cmd_buf[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", hci_cmd_buf[i]);
    }
    printf("\n");
    #endif

    mp_bluetooth_nimble_hci_uart_tx_strn((void*)mp_bluetooth_hci_cmd_buf, len);
}

int hal_uart_close(uint32_t port) {
    return 0; // success
}

void mp_bluetooth_nimble_hci_uart_process(void) {
    mp_bluetooth_nimble_hci_uart_rx(hal_uart_rx_cb, hal_uart_rx_arg);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
