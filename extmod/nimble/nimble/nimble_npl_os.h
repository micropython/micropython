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

#ifndef MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NIMBLE_NPL_OS_H
#define MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NIMBLE_NPL_OS_H

// This is included by nimble/nimble_npl.h -- include that rather than this file directly.

#include <stdint.h>

// --- Configuration of NimBLE data structures --------------------------------

#define BLE_NPL_OS_ALIGNMENT (sizeof(uintptr_t))
#define BLE_NPL_TIME_FOREVER (0xffffffff)

typedef uint32_t ble_npl_time_t;
typedef int32_t ble_npl_stime_t;

struct ble_npl_event {
    ble_npl_event_fn *fn;
    void *arg;
    struct ble_npl_event *prev;
    struct ble_npl_event *next;
};

struct ble_npl_eventq {
    struct ble_npl_event *head;
    struct ble_npl_eventq *nextq;
};

struct ble_npl_callout {
    bool active;
    uint32_t ticks;
    struct ble_npl_eventq *evq;
    struct ble_npl_event ev;
    struct ble_npl_callout *nextc;
};

struct ble_npl_mutex {
    volatile uint8_t locked;
    volatile uint32_t atomic_state;
};

struct ble_npl_sem {
    volatile uint16_t count;
};

// --- Called by the MicroPython port -----------------------------------------

void mp_bluetooth_nimble_os_eventq_run_all(void);
void mp_bluetooth_nimble_os_callout_process(void);

// --- Must be provided by the MicroPython port -------------------------------

void mp_bluetooth_nimble_hci_uart_wfi(void);
uint32_t mp_bluetooth_nimble_hci_uart_enter_critical(void);
void mp_bluetooth_nimble_hci_uart_exit_critical(uint32_t atomic_state);

#endif // MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NPL_OS_H
