/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#include "lib/btstack/platform/embedded/btstack_run_loop_embedded.h"
#include "lib/btstack/platform/embedded/hal_cpu.h"
#include "lib/btstack/platform/embedded/hal_time_ms.h"

#include "extmod/modbluetooth_hci.h"
#include "extmod/btstack/modbluetooth_btstack.h"

// We pass the bytes directly to the UART during a send, but then notify btstack in the next poll.
STATIC bool send_done;
STATIC void (*send_handler)(void);

// btstack issues a read of len bytes, and gives us a buffer to asynchronously fill up.
STATIC uint8_t *recv_buf;
STATIC size_t recv_len;
STATIC size_t recv_idx;
STATIC void (*recv_handler)(void);

// The IRQ functionality in btstack_run_loop_embedded.c is not used, so the
// following three functions are empty.

void hal_cpu_disable_irqs(void) {
}

void hal_cpu_enable_irqs(void) {
}

void hal_cpu_enable_irqs_and_sleep(void) {
}

uint32_t hal_time_ms(void) {
    return mp_hal_ticks_ms();
}

STATIC int btstack_uart_init(const btstack_uart_config_t *uart_config) {
    send_done = false;
    recv_len = 0;
    recv_idx = 0;
    recv_handler = NULL;
    send_handler = NULL;

    // Set up the UART periperhal.
    mp_bluetooth_hci_uart_init(MICROPY_HW_BLE_UART_ID);

    return 0;
}

STATIC int btstack_uart_open(void) {
    // Attach IRQ and power up the HCI controller.
    mp_bluetooth_hci_uart_activate();
    return 0;
}

STATIC int btstack_uart_close(void) {
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
    return 0;
}

STATIC int btstack_uart_set_flowcontrol(int flowcontrol) {
    return 0;
}

STATIC void btstack_uart_receive_block(uint8_t *buf, uint16_t len) {
    recv_buf = buf;
    recv_len = len;
}

STATIC void btstack_uart_send_block(const uint8_t *buf, uint16_t len) {
    mp_bluetooth_hci_uart_write(buf, len);
    send_done = true;
}

STATIC int btstack_uart_get_supported_sleep_modes(void) {
    return 0;
}

STATIC void btstack_uart_set_sleep(btstack_uart_sleep_mode_t sleep_mode) {
    // printf("btstack_uart_set_sleep %u\n", sleep_mode);
}

STATIC void btstack_uart_set_wakeup_handler(void (*wakeup_handler)(void)) {
    // printf("btstack_uart_set_wakeup_handler\n");
}

STATIC const btstack_uart_block_t btstack_uart_block = {
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

STATIC const hci_transport_config_uart_t hci_transport_config_uart = {
    HCI_TRANSPORT_CONFIG_UART,
    MICROPY_HW_BLE_UART_BAUDRATE,
    3000000,
    0,
    NULL,
};

STATIC void btstack_uart_process(void) {
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
            recv_idx = 0;
            recv_len = 0;
            if (recv_handler) {
                recv_handler();
            }
        }
    }
}

void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_OFF) {
        return;
    }

    // Process uart data.
    bool host_wake = mp_bluetooth_hci_controller_woken();
    btstack_uart_process();
    if (host_wake) {
        mp_bluetooth_hci_controller_sleep_maybe();
    }

    // Call the BTstack run loop.
    btstack_run_loop_embedded_execute_once();
}

void mp_bluetooth_btstack_port_init(void) {
    static bool run_loop_init = false;
    if (!run_loop_init) {
        run_loop_init = true;
        btstack_run_loop_init(btstack_run_loop_embedded_get_instance());
    } else {
        btstack_run_loop_embedded_get_instance()->init();
    }

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);
    const hci_transport_t *transport = hci_transport_h4_instance(&btstack_uart_block);
    hci_init(transport, &hci_transport_config_uart);

    // TODO: Probably not necessary for BCM (we have our own firmware loader),
    // but might be worth investigating for other controllers in the future.
    // hci_set_chipset(btstack_chipset_bcm_instance());
}

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);
}

void mp_bluetooth_btstack_port_start(void) {
    hci_power_control(HCI_POWER_ON);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
