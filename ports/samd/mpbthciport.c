/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
#include "extmod/mpbthci.h"
#include "shared/runtime/softtimer.h"
#include "modmachine.h"
#include "mpbthciport.h"
#include "shared/runtime/mpirq.h"

#if MICROPY_PY_BLUETOOTH

#define debug_printf(...) // mp_printf(&mp_plat_print, "mpbthciport.c: " __VA_ARGS__)
#define error_printf(...) mp_printf(&mp_plat_print, "mpbthciport.c: " __VA_ARGS__)

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

// Prevent double-enqueuing of the scheduled task.
STATIC volatile bool events_task_is_scheduled;

// Soft timer for scheduling a HCI poll.
STATIC soft_timer_entry_t mp_bluetooth_hci_soft_timer;

// This is called by soft_timer and executes at IRQ_PRI_PENDSV.
STATIC void mp_bluetooth_hci_soft_timer_callback(soft_timer_entry_t *self) {
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_init(void) {
    soft_timer_static_init(
        &mp_bluetooth_hci_soft_timer,
        SOFT_TIMER_MODE_ONE_SHOT,
        0,
        mp_bluetooth_hci_soft_timer_callback
        );
    events_task_is_scheduled = false;
}

STATIC void mp_bluetooth_hci_start_polling(void) {
    events_task_is_scheduled = false;
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_poll_in_ms(uint32_t ms) {
    soft_timer_reinsert(&mp_bluetooth_hci_soft_timer, ms);
}

#if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

STATIC mp_sched_node_t mp_bluetooth_hci_sched_node;

// For synchronous mode, we run all BLE stack code inside a scheduled task.
// This task is scheduled periodically via a timer, or immediately after UART RX IRQ.
STATIC void run_events_scheduled_task(mp_sched_node_t *node) {
    (void)node;
    events_task_is_scheduled = false;
    // This will process all buffered HCI UART data, and run any callouts or events.
    mp_bluetooth_hci_poll();
}

// Called periodically (systick) or directly (e.g. UART RX IRQ) in order to
// request that processing happens ASAP in the scheduler.
void mp_bluetooth_hci_poll_now(void) {
    if (!events_task_is_scheduled) {
        mp_sched_schedule_node(&mp_bluetooth_hci_sched_node, run_events_scheduled_task);
        events_task_is_scheduled = true;
    }
}

#else // !MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

void mp_bluetooth_hci_poll_now(void) {
    if (!events_task_is_scheduled) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_BLUETOOTH_HCI, mp_bluetooth_hci_poll);
        events_task_is_scheduled = true;
    }
}

#endif // MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

mp_obj_t mp_bthci_uart;
extern mp_obj_t mp_bluetooth_interface_config;
mp_bluetooth_wiring_t mp_bluetooth_wiring = {
    NULL, 0, 0, 0, 0
};

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    debug_printf("mp_bluetooth_hci_uart_init\n");

    if (mp_bluetooth_interface_config != 0) {
        if (mp_obj_is_type(mp_bluetooth_interface_config, &mp_type_tuple)) {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(mp_bluetooth_interface_config, 5, &items);
            if (!(mp_obj_is_type(items[0], &machine_uart_type) &&
                  mp_obj_is_type(items[1], &machine_pin_type) &&
                  mp_obj_is_type(items[2], &machine_pin_type) &&
                  mp_obj_is_type(items[3], &machine_pin_type) &&
                  mp_obj_is_type(items[4], &machine_pin_type))) {
                mp_raise_TypeError(MP_ERROR_TEXT("ble_init - wrong argument type"));
            }
            mp_bthci_uart = items[0];
            mp_bluetooth_wiring.uart = items[0];
            if (items[1] != mp_const_none) {
               mp_bluetooth_wiring.rts = mp_hal_get_pin_obj(items[1]);
            }
            if (items[2] != mp_const_none) {
                mp_bluetooth_wiring.cts = mp_hal_get_pin_obj(items[2]);
            }
            mp_bluetooth_wiring.gpio1 = mp_hal_get_pin_obj(items[3]);
            mp_bluetooth_wiring.reset = mp_hal_get_pin_obj(items[4]);
        } else {
            mp_raise_TypeError(MP_ERROR_TEXT("ble_init - must be a tuple or list")); 
        }
    } else {
        #if defined(MICROPY_HW_BLE_UART_ID) && defined(MICROPY_HW_BLE_UART_TX) && defined(MICROPY_HW_BLE_UART_RX)
        mp_obj_t args[] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_BLE_UART_ID),
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_BLE_UART_BAUDRATE),
            MP_OBJ_NEW_QSTR(MP_QSTR_timeout), MP_OBJ_NEW_SMALL_INT(1000),
            MP_OBJ_NEW_QSTR(MP_QSTR_tx), MP_OBJ_NEW_SMALL_INT(MICROPY_HW_BLE_UART_TX),
            MP_OBJ_NEW_QSTR(MP_QSTR_rx), MP_OBJ_NEW_SMALL_INT(MICROPY_HW_BLE_UART_RX),
        };

        // This is a statically-allocated UART (see machine_uart.c), and doesn't
        // contain any heap pointers other than the ringbufs (which are already
        // root pointers), so no need to track this as a root pointer.
        mp_bthci_uart = MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, make_new)((mp_obj_t)&machine_uart_type, 2, 3, args);
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("UART not defined"));
        #endif
    }
    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();
    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    debug_printf("mp_bluetooth_hci_uart_deinit\n");

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    debug_printf("mp_bluetooth_hci_uart_set_baudrate(%lu)\n", baudrate);
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
