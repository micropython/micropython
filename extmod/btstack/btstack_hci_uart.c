/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "lib/btstack/src/btstack.h"

#include "extmod/mpbthci.h"
#include "extmod/btstack/btstack_hci_uart.h"

#include "mpbtstackport.h"

#define HCI_TRACE (0)
#define COL_OFF "\033[0m"
#define COL_GREEN "\033[0;32m"
#define COL_BLUE "\033[0;34m"

// Implements a btstack btstack_uart_block_t on top of the mphciuart.h
// interface to an HCI UART provided by the port.

// We pass the bytes directly to the UART during a send, but then notify btstack in the next poll.
STATIC bool send_done;
STATIC void (*send_handler)(void);

// btstack issues a read of len bytes, and gives us a buffer to asynchronously fill up.
STATIC uint8_t *recv_buf;
STATIC size_t recv_len;
STATIC size_t recv_idx;
STATIC void (*recv_handler)(void);
STATIC bool init_success = false;

STATIC int btstack_uart_init(const btstack_uart_config_t *uart_config) {
    (void)uart_config;

    send_done = false;
    recv_len = 0;
    recv_idx = 0;
    recv_handler = NULL;
    send_handler = NULL;

    // Set up the UART peripheral, attach IRQ and power up the HCI controller.
    if (mp_bluetooth_hci_uart_init(MICROPY_HW_BLE_UART_ID, MICROPY_HW_BLE_UART_BAUDRATE)) {
        init_success = false;
        return -1;
    }
    if (mp_bluetooth_hci_controller_init()) {
        init_success = false;
        return -1;
    }

    init_success = true;
    return 0;
}

STATIC int btstack_uart_open(void) {
    return init_success ? 0 : 1;
}

STATIC int btstack_uart_close(void) {
    mp_bluetooth_hci_controller_deinit();
    return 0;
}

STATIC void btstack_uart_set_block_received(void (*block_handler)(void)) {
    recv_handler = block_handler;
}

STATIC void btstack_uart_set_block_sent(void (*block_handler)(void)) {
    send_handler = block_handler;
}

STATIC int btstack_uart_set_baudrate(uint32_t baudrate) {
    mp_bluetooth_hci_uart_set_baudrate(baudrate);
    return 0;
}

STATIC int btstack_uart_set_parity(int parity) {
    (void)parity;
    return 0;
}

STATIC int btstack_uart_set_flowcontrol(int flowcontrol) {
    (void)flowcontrol;
    return 0;
}

STATIC void btstack_uart_receive_block(uint8_t *buf, uint16_t len) {
    recv_buf = buf;
    recv_len = len;
}

STATIC void btstack_uart_send_block(const uint8_t *buf, uint16_t len) {
    #if HCI_TRACE
    printf(COL_GREEN "< [% 8d] %02x", (int)mp_hal_ticks_ms(), buf[0]);
    for (size_t i = 1; i < len; ++i) {
        printf(":%02x", buf[i]);
    }
    printf(COL_OFF "\n");
    #endif

    mp_bluetooth_hci_uart_write(buf, len);
    send_done = true;
}

STATIC int btstack_uart_get_supported_sleep_modes(void) {
    return 0;
}

STATIC void btstack_uart_set_sleep(btstack_uart_sleep_mode_t sleep_mode) {
    (void)sleep_mode;
    // printf("btstack_uart_set_sleep %u\n", sleep_mode);
}

STATIC void btstack_uart_set_wakeup_handler(void (*wakeup_handler)(void)) {
    (void)wakeup_handler;
    // printf("btstack_uart_set_wakeup_handler\n");
}

const btstack_uart_block_t mp_bluetooth_btstack_hci_uart_block = {
    &btstack_uart_init,
    &btstack_uart_open,
    &btstack_uart_close,
    &btstack_uart_set_block_received,
    &btstack_uart_set_block_sent,
    &btstack_uart_set_baudrate,
    &btstack_uart_set_parity,
    &btstack_uart_set_flowcontrol,
    &btstack_uart_receive_block,
    &btstack_uart_send_block,
    &btstack_uart_get_supported_sleep_modes,
    &btstack_uart_set_sleep,
    &btstack_uart_set_wakeup_handler,
};

void mp_bluetooth_btstack_hci_uart_process(void) {
    bool host_wake = mp_bluetooth_hci_controller_woken();

    if (send_done) {
        // If we'd done a TX in the last interval, notify btstack that it's complete.
        send_done = false;
        if (send_handler) {
            send_handler();
        }
    }

    // Append any new bytes to the recv buffer, notifying bstack if we've got
    // the number of bytes it was looking for.
    int chr;
    while (recv_idx < recv_len && (chr = mp_bluetooth_hci_uart_readchar()) >= 0) {
        recv_buf[recv_idx++] = chr;
        if (recv_idx == recv_len) {
            #if HCI_TRACE
            printf(COL_BLUE "> [% 8d] %02x", (int)mp_hal_ticks_ms(), recv_buf[0]);
            for (size_t i = 1; i < recv_len; ++i) {
                printf(":%02x", recv_buf[i]);
            }
            printf(COL_OFF "\n");
            #endif
            recv_idx = 0;
            recv_len = 0;
            if (recv_handler) {
                recv_handler();
            }
        }
    }

    if (host_wake) {
        mp_bluetooth_hci_controller_sleep_maybe();
    }
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
