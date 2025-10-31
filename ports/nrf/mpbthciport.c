/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jim Mussared
 * Copyright (c) 2020 Damien P. George
 * Copyright (c) 2022 Andrew Leech
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
#include "shared/runtime/softtimer.h"

#if MICROPY_PY_BLUETOOTH

#define DEBUG_printf(...) // mp_printf(&mp_plat_print, "mpbthciport.c: " __VA_ARGS__)

#include "mpbthciport.h"
// #include "drivers/ticker.h"

// #define BLUETOOTH_TICKER_SLOT 0

// Soft timer and scheduling node for scheduling a HCI poll.
static soft_timer_entry_t mp_bluetooth_hci_soft_timer;
static mp_sched_node_t mp_bluetooth_hci_sched_node;


// This is called by soft_timer and executes at PendSV level.
static void mp_bluetooth_hci_soft_timer_callback(soft_timer_entry_t *self) {
    mp_bluetooth_hci_poll_now();
}

// // Prevent double-enqueuing of the scheduled task.
// static volatile bool events_task_is_scheduled = false;

void mp_bluetooth_hci_init(void) {
    /* Start regular background task to handle events */
    // events_task_is_scheduled = false;
    // set_ticker_callback(BLUETOOTH_TICKER_SLOT, mp_bluetooth_hci_poll_now, 0);
    soft_timer_static_init(
        &mp_bluetooth_hci_soft_timer,
        SOFT_TIMER_MODE_ONE_SHOT,
        0,
        mp_bluetooth_hci_soft_timer_callback
        );
}

void mp_bluetooth_hci_deinit(void) {
    // clear_ticker_callback(BLUETOOTH_TICKER_SLOT);
    //   events_task_is_scheduled = false;
    soft_timer_remove(&mp_bluetooth_hci_soft_timer);

}

// // For synchronous mode, we run all BLE stack code inside a scheduled task.
// // This task is scheduled periodically via a timer.
// static mp_obj_t run_events_scheduled_task(mp_obj_t none_in) {
//     (void)none_in;
//     events_task_is_scheduled = false;
//     mp_bluetooth_hci_poll();
//     return mp_const_none;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(run_events_scheduled_task_obj, run_events_scheduled_task);


// // Called periodically (ticker) to request that processing happens in the scheduler.
// int32_t mp_bluetooth_hci_poll_now(void) {
//     // Return interval (128ms in 16us ticks) until next callback run
//     uint32_t next_tick = 128000 / 16;
//     if (!events_task_is_scheduled) {
//         events_task_is_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&run_events_scheduled_task_obj), mp_const_none);
//         if (!events_task_is_scheduled) {
//             // The schedule queue is full, set callback to try again soon (5ms).
//             next_tick = 5000 / 16;
//         }
//     }
//     return next_tick;
// }

// static void mp_bluetooth_hci_start_polling(void) {
//     mp_bluetooth_hci_poll_now();
// }

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

#endif // MICROPY_PY_BLUETOOTH
