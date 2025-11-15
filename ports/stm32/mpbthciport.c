/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2021 Damien P. George
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
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/mpbthci.h"
#include "extmod/modbluetooth.h"
#include "shared/runtime/softtimer.h"
#include "mpbthciport.h"
#include "pendsv.h"
#include "shared/runtime/mpirq.h"

#if MICROPY_PY_BLUETOOTH

#define DEBUG_printf(...) // printf("mpbthciport.c: " __VA_ARGS__)

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

// Soft timer for scheduling a HCI poll.
static soft_timer_entry_t mp_bluetooth_hci_soft_timer;

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

void mp_bluetooth_hci_poll_in_ms_default(uint32_t ms) {
    soft_timer_reinsert(&mp_bluetooth_hci_soft_timer, ms);
}

#if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

static mp_sched_node_t mp_bluetooth_hci_sched_node;

// For synchronous mode, we run all BLE stack code inside a scheduled task.
// This task is scheduled periodically via a soft timer, or
// immediately on HCI UART RXIDLE.
static void run_events_scheduled_task(mp_sched_node_t *node) {
    // This will process all buffered HCI UART data, and run any callouts or events.
    (void)node;
    mp_bluetooth_hci_poll();
}

// Called periodically (systick) or directly (e.g. UART RX IRQ) in order to
// request that processing happens ASAP in the scheduler.
void mp_bluetooth_hci_poll_now_default(void) {
    mp_sched_schedule_node(&mp_bluetooth_hci_sched_node, run_events_scheduled_task);
}

#else // !MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

void mp_bluetooth_hci_poll_now_default(void) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_BLUETOOTH_HCI, mp_bluetooth_hci_poll);
}

#endif

#if defined(STM32WB)

/******************************************************************************/
// HCI over IPCC

#include <string.h>
#include "rfcore.h"

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    (void)port;
    (void)baudrate;

    DEBUG_printf("mp_bluetooth_hci_uart_init (stm32 rfcore)\n");

    rfcore_ble_init();

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit (stm32 rfcore)\n");
    rfcore_ble_reset();
    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    (void)baudrate;
    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    MICROPY_PY_BLUETOOTH_ENTER
    rfcore_ble_hci_cmd(len, (const uint8_t *)buf);
    MICROPY_PY_BLUETOOTH_EXIT
    return 0;
}

// Callback to forward data from rfcore to the bluetooth hci handler.
static void mp_bluetooth_hci_uart_msg_cb(void *env, const uint8_t *buf, size_t len) {
    mp_bluetooth_hci_uart_readchar_t handler = (mp_bluetooth_hci_uart_readchar_t)env;
    for (size_t i = 0; i < len; ++i) {
        handler(buf[i]);
    }
}

int mp_bluetooth_hci_uart_readpacket(mp_bluetooth_hci_uart_readchar_t handler) {
    size_t len = rfcore_ble_check_msg(mp_bluetooth_hci_uart_msg_cb, (void *)handler);
    return (len > 0) ? len : -1;
}

#else

/******************************************************************************/
// HCI over UART

#include "extmod/modmachine.h"
#include "uart.h"

machine_uart_obj_t mp_bluetooth_hci_uart_obj;
mp_irq_obj_t mp_bluetooth_hci_uart_irq_obj;

static uint8_t hci_uart_rxbuf[768];

mp_obj_t mp_uart_interrupt(mp_obj_t self_in) {
    // Queue up the scheduler to run the HCI UART and event processing ASAP.
    mp_bluetooth_hci_poll_now();

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_uart_interrupt_obj, mp_uart_interrupt);

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    DEBUG_printf("mp_bluetooth_hci_uart_init (stm32)\n");

    // bits (8), stop (1), parity (none) and flow (rts/cts) are assumed to match MYNEWT_VAL_BLE_HCI_UART_ constants in syscfg.h.
    #if MICROPY_PY_MACHINE_UART
    mp_bluetooth_hci_uart_obj.base.type = &machine_uart_type;
    #else
    // With machine.UART disabled this object is not user-accessible so doesn't need a type.
    mp_bluetooth_hci_uart_obj.base.type = NULL;
    #endif
    mp_bluetooth_hci_uart_obj.uart_id = port;
    mp_bluetooth_hci_uart_obj.is_static = true;
    // We don't want to block indefinitely, but expect flow control is doing its job.
    mp_bluetooth_hci_uart_obj.timeout = 200;
    mp_bluetooth_hci_uart_obj.timeout_char = 200;
    MP_STATE_PORT(machine_uart_obj_all)[mp_bluetooth_hci_uart_obj.uart_id - 1] = &mp_bluetooth_hci_uart_obj;

    // Initialise the UART.
    uart_init(&mp_bluetooth_hci_uart_obj, baudrate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, UART_HWCONTROL_RTS | UART_HWCONTROL_CTS, 0);
    uart_set_rxbuf(&mp_bluetooth_hci_uart_obj, sizeof(hci_uart_rxbuf), hci_uart_rxbuf);

    // Add IRQ handler for IDLE (i.e. packet finished).
    uart_irq_config(&mp_bluetooth_hci_uart_obj, false);
    mp_irq_init(&mp_bluetooth_hci_uart_irq_obj, &uart_irq_methods, MP_OBJ_FROM_PTR(&mp_bluetooth_hci_uart_obj));
    mp_bluetooth_hci_uart_obj.mp_irq_obj = &mp_bluetooth_hci_uart_irq_obj;
    mp_bluetooth_hci_uart_obj.mp_irq_trigger = UART_FLAG_IDLE;
    mp_bluetooth_hci_uart_irq_obj.handler = MP_OBJ_FROM_PTR(&mp_uart_interrupt_obj);
    mp_bluetooth_hci_uart_irq_obj.ishard = true;
    uart_irq_config(&mp_bluetooth_hci_uart_obj, true);

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit (stm32)\n");

    // TODO: deinit mp_bluetooth_hci_uart_obj

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    DEBUG_printf("mp_bluetooth_hci_uart_set_baudrate(%lu) (stm32)\n", baudrate);
    uart_set_baudrate(&mp_bluetooth_hci_uart_obj, baudrate);
    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    // DEBUG_printf("mp_bluetooth_hci_uart_write (stm32)\n");

    mp_bluetooth_hci_controller_wakeup();
    int errcode;
    uart_tx_data(&mp_bluetooth_hci_uart_obj, (void *)buf, len, &errcode);
    if (errcode != 0) {
        mp_printf(&mp_plat_print, "\nmp_bluetooth_hci_uart_write: failed to write to UART %d\n", errcode);
    }
    return 0;
}

// This function expects the controller to be in the wake state via a previous call
// to mp_bluetooth_hci_controller_woken.
int mp_bluetooth_hci_uart_readchar(void) {
    // DEBUG_printf("mp_bluetooth_hci_uart_readchar (stm32)\n");

    if (uart_rx_any(&mp_bluetooth_hci_uart_obj)) {
        // DEBUG_printf("... available\n");
        return uart_rx_char(&mp_bluetooth_hci_uart_obj);
    } else {
        return -1;
    }
}

#endif // defined(STM32WB)

// Default (weak) implementation of the HCI controller interface.
// A driver (e.g. cywbt43.c) can override these for controller-specific
// functionality (i.e. power management).

MP_WEAK int mp_bluetooth_hci_controller_init(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_init (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_deinit (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_sleep_maybe(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_sleep_maybe (default)\n");
    return 0;
}

MP_WEAK bool mp_bluetooth_hci_controller_woken(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_woken (default)\n");
    return true;
}

MP_WEAK int mp_bluetooth_hci_controller_wakeup(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_wakeup (default)\n");
    return 0;
}

#endif // MICROPY_PY_BLUETOOTH
