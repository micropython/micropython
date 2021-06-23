/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYMATRIX_H
#define MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYMATRIX_H

#include "py/obj.h"
#include "py/objtuple.h"

#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/keypad/__init__.h"
#include "shared-module/keypad/EventQueue.h"

typedef struct {
    mp_obj_base_t base;
    // All scanners have a next field here, to keep a linked list of active scanners.
    keypad_scanner_obj_t *next;
    mp_obj_tuple_t *row_digitalinouts;
    mp_obj_tuple_t *column_digitalinouts;
    mp_uint_t interval_ticks;
    uint64_t last_scan_ticks;
    bool *previously_pressed;
    bool *currently_pressed;
    keypad_eventqueue_obj_t *events;
    bool columns_to_anodes;
} keypad_keymatrix_obj_t;

void keypad_keymatrix_scan(keypad_keymatrix_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYMATRIX_H
