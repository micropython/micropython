/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 OpenMV LLC.
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
#include "py/runtime.h"
#include "extmod/mpbthci.h"
#include "shared/runtime/softtimer.h"
#include "mpbthciport.h"

#if MICROPY_PY_BLUETOOTH

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

// Soft timer and scheduling node for scheduling a HCI poll.
static soft_timer_entry_t mp_bluetooth_hci_soft_timer;
static mp_sched_node_t mp_bluetooth_hci_sched_node;

// This is called by soft_timer and executes at IRQ_PRI_PENDSV.
static void mp_bluetooth_hci_soft_timer_callback(soft_timer_entry_t *self) {
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_init(void) {
    soft_timer_static_init(
        &mp_bluetooth_hci_soft_timer,
        SOFT_TIMER_MODE_ONE_SHOT,
        0,
        mp_bluetooth_hci_soft_timer_callback
        );
}

static void mp_bluetooth_hci_start_polling(void) {
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_poll_in_ms(uint32_t ms) {
    soft_timer_reinsert(&mp_bluetooth_hci_soft_timer, ms);
}

// For synchronous mode, we run all BLE stack code inside a scheduled task.
// This task is scheduled periodically via a timer, or immediately after UART RX IRQ.
static void run_events_scheduled_task(mp_sched_node_t *node) {
    (void)node;
    // This will process all buffered HCI UART data, and run any callouts or events.
    mp_bluetooth_hci_poll();
}

// Called periodically (systick) or directly (e.g. UART RX IRQ) in order to
// request that processing happens ASAP in the scheduler.
void mp_bluetooth_hci_poll_now(void) {
    mp_sched_schedule_node(&mp_bluetooth_hci_sched_node, run_events_scheduled_task);
}

/******************************************************************************/
// HCI over UART

#include "mpuart.h"

static uint32_t hci_uart_id;
static bool hci_uart_first_char;
static uint8_t hci_rx_ringbuf_array[768];
static ringbuf_t hci_rx_ringbuf = {
    .buf = hci_rx_ringbuf_array,
    .size = sizeof(hci_rx_ringbuf_array),
    .iget = 0,
    .iput = 0,
};

static void mp_bluetooth_hci_uart_irq_callback(unsigned int uart_id, unsigned int trigger) {
    if (trigger == MP_UART_IRQ_RXIDLE) {
        mp_bluetooth_hci_poll_now();
    }
}

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    hci_uart_id = port;
    hci_uart_first_char = true;

    // Initialise the UART.
    mp_uart_init(hci_uart_id, baudrate, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1, pin_BT_UART_TX, pin_BT_UART_RX, &hci_rx_ringbuf);
    mp_uart_set_flow(hci_uart_id, pin_BT_UART_RTS, pin_BT_UART_CTS);
    mp_uart_set_irq_callback(hci_uart_id, MP_UART_IRQ_RXIDLE, mp_bluetooth_hci_uart_irq_callback);

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    mp_uart_deinit(hci_uart_id);
    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    mp_uart_set_baudrate(hci_uart_id, baudrate);
    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    mp_bluetooth_hci_controller_wakeup();
    mp_uart_tx_data(hci_uart_id, (void *)buf, len);
    return 0;
}

// This function expects the controller to be in the wake state via a previous call
// to mp_bluetooth_hci_controller_woken.
int mp_bluetooth_hci_uart_readchar(void) {
    if (mp_uart_rx_any(hci_uart_id)) {
        int c = mp_uart_rx_char(hci_uart_id);
        if (hci_uart_first_char) {
            if (c == 0) {
                return -1;
            }
            hci_uart_first_char = false;
        }
        return c;
    } else {
        return -1;
    }
}

#endif // MICROPY_PY_BLUETOOTH
