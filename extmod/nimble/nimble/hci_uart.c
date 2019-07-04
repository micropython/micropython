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
#include "hal/hal_uart.h"
#include "hci_uart.h"

#if MICROPY_BLUETOOTH_NIMBLE

/******************************************************************************/
// Bindings Uart to Nimble
uint8_t bt_hci_cmd_buf[4 + 256];

static hal_uart_tx_cb_t hal_uart_tx_cb;
static void *hal_uart_tx_arg;
static hal_uart_rx_cb_t hal_uart_rx_cb;
static void *hal_uart_rx_arg;

static uint32_t bt_sleep_ticks;

int hal_uart_init_cbs(uint32_t port, hal_uart_tx_cb_t tx_cb, void *tx_arg, hal_uart_rx_cb_t rx_cb, void *rx_arg) {
    hal_uart_tx_cb = tx_cb;
    hal_uart_tx_arg = tx_arg;
    hal_uart_rx_cb = rx_cb;
    hal_uart_rx_arg = rx_arg;
    return 0; // success
}

int hal_uart_config(uint32_t port, uint32_t baudrate, uint32_t bits, uint32_t stop, uint32_t parity, uint32_t flow) {
    nimble_hci_uart_configure(port);
    nimble_hci_uart_set_baudrate(baudrate);
    return nimble_hci_uart_activate();
}

void hal_uart_start_tx(uint32_t port) {
    size_t len = 0;
    for (;;) {
        int data = hal_uart_tx_cb(hal_uart_tx_arg);
        if (data == -1) {
            break;
        }
        bt_hci_cmd_buf[len++] = data;
    }

    #if 0
    printf("[% 8d] BTUTX: %02x", mp_hal_ticks_ms(), hci_cmd_buf[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", hci_cmd_buf[i]);
    }
    printf("\n");
    #endif

    bt_sleep_ticks = mp_hal_ticks_ms();

    #ifdef pyb_pin_BT_DEV_WAKE
        if (mp_hal_pin_read(pyb_pin_BT_DEV_WAKE) == 1) {
            //printf("BT WAKE for TX\n");
            mp_hal_pin_low(pyb_pin_BT_DEV_WAKE); // wake up
            mp_hal_delay_ms(5); // can't go lower than this
        }
    #endif

    nimble_hci_uart_tx_strn((void*)bt_hci_cmd_buf, len);
}

int hal_uart_close(uint32_t port) {
    return 0; // success
}

void nimble_uart_process(void) {
    int host_wake = 0;
    #ifdef pyb_pin_BT_HOST_WAKE
        host_wake = mp_hal_pin_read(pyb_pin_BT_HOST_WAKE);
    #endif
    /*
    // this is just for info/tracing purposes
    static int last_host_wake = 0;
    if (host_wake != last_host_wake) {
        printf("HOST_WAKE change %d -> %d\n", last_host_wake, host_wake);
        last_host_wake = host_wake;
    }
    */
    while (nimble_hci_uart_rx_any()) {
        uint8_t data = nimble_hci_uart_rx_char();
        //printf("UART RX: %02x\n", data);
        hal_uart_rx_cb(hal_uart_rx_arg, data);
    }
    if (host_wake == 1 && mp_hal_pin_read(pyb_pin_BT_DEV_WAKE) == 0) {
        if (mp_hal_ticks_ms() - bt_sleep_ticks > 500) {
            //printf("BT SLEEP\n");
            mp_hal_pin_high(pyb_pin_BT_DEV_WAKE); // let sleep
        }
    }
}

#endif // MICROPY_BLUETOOTH_NIMBLE
