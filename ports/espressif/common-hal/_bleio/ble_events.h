/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL__BLEIO_BLE_EVENTS_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL__BLEIO_BLE_EVENTS_H

#include <stdbool.h>

#include "host/ble_gap.h"

typedef struct ble_event_handler_entry {
    struct ble_event_handler_entry *next;
    void *param;
    ble_gap_event_fn *func;
} ble_event_handler_entry_t;

void ble_event_reset(void);
void ble_event_add_handler(ble_gap_event_fn *func, void *param);
void ble_event_remove_handler(ble_gap_event_fn *func, void *param);

// Allow for user provided entries to prevent allocations outside the VM.
void ble_event_add_handler_entry(ble_event_handler_entry_t *entry, ble_gap_event_fn *func, void *param);

int ble_event_run_handlers(struct ble_gap_event *event);

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL__BLEIO_BLE_EVENTS_H
