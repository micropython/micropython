/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#define DEBUG_printf(...)
#define BTSTACK_ENABLE_HCI_DUMP (0)

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "lib/btstack/src/btstack.h"
#include "lib/btstack/platform/embedded/hci_dump_embedded_stdout.h"
#include "extmod/mpbthci.h"
#include "extmod/btstack/modbluetooth_btstack.h"
#include "mpbthciport.h"
#include "pico/btstack_hci_transport_cyw43.h"

void mp_bluetooth_hci_poll_in_ms(uint32_t ms);

static btstack_linked_list_t mp_btstack_runloop_timers;

static void mp_btstack_runloop_init(void) {
    mp_btstack_runloop_timers = NULL;
}

static void mp_btstack_runloop_set_timer(btstack_timer_source_t *tim, uint32_t timeout_ms) {
    DEBUG_printf("mp_btstack_runloop_set_timer %lu\n", timeout_ms);
    tim->timeout = mp_hal_ticks_ms() + timeout_ms + 1;
}

static void mp_btstack_runloop_add_timer(btstack_timer_source_t *tim) {
    DEBUG_printf("mp_btstack_runloop_add_timer %lu\n", tim->timeout);
    btstack_linked_item_t **node = &mp_btstack_runloop_timers;
    for (; *node; node = &(*node)->next) {
        btstack_timer_source_t *node_tim = (btstack_timer_source_t *)*node;
        if (node_tim == tim) {
            // Timer is already in the list, don't add it.
            return;
        }
        int32_t delta = btstack_time_delta(tim->timeout, node_tim->timeout);
        if (delta < 0) {
            // Found sorted location in list.
            break;
        }
    }

    // Insert timer into list in sorted location.
    tim->item.next = *node;
    *node = &tim->item;

    // Reschedule the HCI poll if this timer is at the head of the list.
    if (mp_btstack_runloop_timers == &tim->item) {
        int32_t delta_ms = btstack_time_delta(tim->timeout, mp_hal_ticks_ms());
        mp_bluetooth_hci_poll_in_ms(delta_ms);
    }
}

static bool mp_btstack_runloop_remove_timer(btstack_timer_source_t *tim) {
    return btstack_linked_list_remove(&mp_btstack_runloop_timers, (btstack_linked_item_t *)tim);
}

static uint32_t mp_btstack_runloop_get_time_ms(void) {
    return mp_hal_ticks_ms();
}

static const btstack_run_loop_t mp_btstack_runloop_rp2 = {
    &mp_btstack_runloop_init,
    &btstack_run_loop_base_add_data_source,
    &btstack_run_loop_base_remove_data_source,
    &btstack_run_loop_base_enable_data_source_callbacks,
    &btstack_run_loop_base_disable_data_source_callbacks,
    &mp_btstack_runloop_set_timer,
    &mp_btstack_runloop_add_timer,
    &mp_btstack_runloop_remove_timer,
    NULL, // execute
    NULL, // dump_timer
    &mp_btstack_runloop_get_time_ms,
    &mp_bluetooth_hci_poll_now, // poll_data_sources_from_irq
    NULL, // execute_on_main_thread
    NULL, // trigger_exit
};

void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_OFF) {
        return;
    }

    // Process BT data.
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_HALTING) {
        btstack_run_loop_base_poll_data_sources();
    }

    // Process any BTstack timers.
    while (mp_btstack_runloop_timers != NULL) {
        btstack_timer_source_t *tim = (btstack_timer_source_t *)mp_btstack_runloop_timers;
        int32_t delta_ms = btstack_time_delta(tim->timeout, mp_hal_ticks_ms());
        if (delta_ms > 0) {
            // Timer has not expired yet, reschedule HCI poll for this timer.
            mp_bluetooth_hci_poll_in_ms(delta_ms);
            break;
        }
        btstack_linked_list_pop(&mp_btstack_runloop_timers);
        tim->process(tim);
    }
}

void mp_bluetooth_btstack_port_init(void) {
    btstack_run_loop_init(&mp_btstack_runloop_rp2);

    #if BTSTACK_ENABLE_HCI_DUMP
    hci_dump_init(hci_dump_embedded_stdout_get_instance());
    #endif

    const hci_transport_t *transport = hci_transport_cyw43_instance();
    hci_init(transport, NULL);
}

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);
    // We really need hci to be closed or else we might get a BT interrupt going off and
    // this will cause a crash when calling btstack_run_loop_poll_data_sources_from_irq after the btstack run loop has been deleted
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_OFF) {
        hci_transport_cyw43_instance()->close();
    }
    hci_close();
}

void mp_bluetooth_btstack_port_start(void) {
    hci_power_control(HCI_POWER_ON);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
