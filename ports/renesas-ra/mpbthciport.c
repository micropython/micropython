/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/modbluetooth.h"
#include "extmod/modmachine.h"
#include "extmod/mpbthci.h"
#include "shared/runtime/softtimer.h"
#include "mpbthciport.h"
#include "uart.h"

#if MICROPY_PY_BLUETOOTH

#define debug_printf(...) // mp_printf(&mp_plat_print, "mpbthciport.c: " __VA_ARGS__)
#define error_printf(...) mp_printf(&mp_plat_print, "mpbthciport.c: " __VA_ARGS__)

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

static mp_sched_node_t mp_bluetooth_hci_sched_node;
static soft_timer_entry_t mp_bluetooth_hci_soft_timer;

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
static void run_events_scheduled_task(mp_sched_node_t *node) {
    // This will process all buffered HCI UART data, and run any callouts or events.
    mp_bluetooth_hci_poll();
}

// Called periodically (systick) or directly (e.g. UART RX IRQ) in order to
// request that processing happens ASAP in the scheduler.
void mp_bluetooth_hci_poll_now(void) {
    mp_sched_schedule_node(&mp_bluetooth_hci_sched_node, run_events_scheduled_task);
}

mp_obj_t mp_bthci_uart;

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    debug_printf("mp_bluetooth_hci_uart_init\n");

    mp_obj_t args[] = {
        MP_OBJ_NEW_SMALL_INT(port),
        MP_OBJ_NEW_QSTR(MP_QSTR_baudrate), MP_OBJ_NEW_SMALL_INT(baudrate),
        MP_OBJ_NEW_QSTR(MP_QSTR_timeout), MP_OBJ_NEW_SMALL_INT(200),
        MP_OBJ_NEW_QSTR(MP_QSTR_timeout_char), MP_OBJ_NEW_SMALL_INT(200),
        MP_OBJ_NEW_QSTR(MP_QSTR_rxbuf), MP_OBJ_NEW_SMALL_INT(768),
        MP_OBJ_NEW_QSTR(MP_QSTR_flow), MP_OBJ_NEW_SMALL_INT(UART_HWCONTROL_RTS | UART_HWCONTROL_CTS),
    };

    // This object is tracked in machine_uart_obj_all root pointer.
    mp_bthci_uart = MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, make_new)((mp_obj_t)&machine_uart_type, 1, 5, args);

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();
    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    debug_printf("mp_bluetooth_hci_uart_deinit\n");
    mp_bthci_uart = MP_OBJ_NULL;
    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    debug_printf("mp_bluetooth_hci_uart_set_baudrate(%lu)\n", baudrate);
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(mp_bthci_uart);
    ra_sci_set_baud((uint32_t)self->uart_id, baudrate);
    return 0;
}

int mp_bluetooth_hci_uart_any(void) {
    int errcode = 0;
    const mp_stream_p_t *proto = (mp_stream_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, protocol);

    mp_uint_t ret = proto->ioctl(mp_bthci_uart, MP_STREAM_POLL, MP_STREAM_POLL_RD, &errcode);
    if (errcode != 0) {
        error_printf("Uart ioctl failed to poll UART %d\n", errcode);
        return -1;
    }
    return ret & MP_STREAM_POLL_RD;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    debug_printf("mp_bluetooth_hci_uart_write\n");

    int errcode = 0;
    const mp_stream_p_t *proto = (mp_stream_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, protocol);

    mp_bluetooth_hci_controller_wakeup();

    if (proto->write(mp_bthci_uart, (void *)buf, len, &errcode) < 0) {
        error_printf("mp_bluetooth_hci_uart_write: failed to write to UART %d\n", errcode);
    }
    return 0;
}

// This function expects the controller to be in the wake state via a previous call
// to mp_bluetooth_hci_controller_woken.
int mp_bluetooth_hci_uart_readchar(void) {
    debug_printf("mp_bluetooth_hci_uart_readchar\n");
    if (mp_bluetooth_hci_uart_any()) {
        int errcode = 0;
        uint8_t buf = 0;
        const mp_stream_p_t *proto = (mp_stream_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, protocol);
        if (proto->read(mp_bthci_uart, (void *)&buf, 1, &errcode) < 0) {
            error_printf("mp_bluetooth_hci_uart_readchar: failed to read UART %d\n", errcode);
            return -1;
        }
        return buf;
    } else {
        debug_printf("mp_bluetooth_hci_uart_readchar: not ready\n");
        return -1;
    }
}

// Default (weak) implementation of the HCI controller interface.
// A driver (e.g. cywbt43.c) can override these for controller-specific
// functionality (i.e. power management).
MP_WEAK int mp_bluetooth_hci_controller_init(void) {
    debug_printf("mp_bluetooth_hci_controller_init (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_deinit(void) {
    debug_printf("mp_bluetooth_hci_controller_deinit (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_sleep_maybe(void) {
    debug_printf("mp_bluetooth_hci_controller_sleep_maybe (default)\n");
    return 0;
}

MP_WEAK bool mp_bluetooth_hci_controller_woken(void) {
    debug_printf("mp_bluetooth_hci_controller_woken (default)\n");
    return true;
}

MP_WEAK int mp_bluetooth_hci_controller_wakeup(void) {
    debug_printf("mp_bluetooth_hci_controller_wakeup (default)\n");
    return 0;
}

#endif // MICROPY_PY_BLUETOOTH
