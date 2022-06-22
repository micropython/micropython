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
#include "nimble/ble.h"
#include "extmod/nimble/modbluetooth_nimble.h"
#include "extmod/nimble/hal/hal_uart.h"
#include "extmod/nimble/nimble/nimble_npl_os.h"
#include "extmod/mpbthci.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

#ifndef MICROPY_PY_BLUETOOTH_HCI_READ_MODE
#define MICROPY_PY_BLUETOOTH_HCI_READ_MODE MICROPY_PY_BLUETOOTH_HCI_READ_MODE_BYTE
#endif

#define HCI_TRACE (0)

static hal_uart_tx_cb_t hal_uart_tx_cb;
static void *hal_uart_tx_arg;
static hal_uart_rx_cb_t hal_uart_rx_cb;
static void *hal_uart_rx_arg;

// Provided by the port, and also possibly shared with the driver.
extern uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

int hal_uart_init_cbs(uint32_t port, hal_uart_tx_cb_t tx_cb, void *tx_arg, hal_uart_rx_cb_t rx_cb, void *rx_arg) {
    hal_uart_tx_cb = tx_cb;
    hal_uart_tx_arg = tx_arg;
    hal_uart_rx_cb = rx_cb;
    hal_uart_rx_arg = rx_arg;
    return 0; // success
}

int hal_uart_config(uint32_t port, uint32_t baudrate, uint32_t bits, uint32_t stop, uint32_t parity, uint32_t flow) {
    return mp_bluetooth_hci_uart_init(port, baudrate);
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

    #if HCI_TRACE
    printf("< [% 8d] %02x", (int)mp_hal_ticks_ms(), mp_bluetooth_hci_cmd_buf[0]);
    for (size_t i = 1; i < len; ++i) {
        printf(":%02x", mp_bluetooth_hci_cmd_buf[i]);
    }
    printf("\n");
    #endif

    mp_bluetooth_hci_uart_write(mp_bluetooth_hci_cmd_buf, len);

    if (len > 0) {
        // Allow modbluetooth bindings to hook "sent packet" (e.g. to unstall l2cap channels).
        mp_bluetooth_nimble_sent_hci_packet();
    }
}

int hal_uart_close(uint32_t port) {
    return 0; // success
}

STATIC void mp_bluetooth_hci_uart_char_cb(uint8_t chr) {
    #if HCI_TRACE
    printf("> %02x\n", chr);
    #endif
    hal_uart_rx_cb(hal_uart_rx_arg, chr);
}

void mp_bluetooth_nimble_hci_uart_process(bool run_events) {
    bool host_wake = mp_bluetooth_hci_controller_woken();

    for (;;) {
        #if MICROPY_PY_BLUETOOTH_HCI_READ_MODE == MICROPY_PY_BLUETOOTH_HCI_READ_MODE_BYTE
        int chr = mp_bluetooth_hci_uart_readchar();
        if (chr < 0) {
            break;
        }
        mp_bluetooth_hci_uart_char_cb(chr);
        #elif MICROPY_PY_BLUETOOTH_HCI_READ_MODE == MICROPY_PY_BLUETOOTH_HCI_READ_MODE_PACKET
        if (mp_bluetooth_hci_uart_readpacket(mp_bluetooth_hci_uart_char_cb) < 0) {
            break;
        }
        #endif

        // Incoming data may result in events being enqueued. If we're in
        // scheduler context then we can run those events immediately.
        if (run_events) {
            mp_bluetooth_nimble_os_eventq_run_all();
        }
    }

    if (host_wake) {
        mp_bluetooth_hci_controller_sleep_maybe();
    }
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
