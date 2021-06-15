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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYS_H
#define MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYS_H

#include "common-hal/digitalio/DigitalInOut.h"

#include "py/obj.h"
#include "py/objtuple.h"
#include "py/ringbuf.h"

typedef struct _keypad_keys_obj_t {
    mp_obj_base_t base;
    mp_obj_tuple_t *digitalinouts;
    uint64_t last_scan_ticks;
    bool value_when_pressed;
    bool *previously_pressed;
    bool *currently_pressed;
    ringbuf_t encoded_events;
    // Keep a linked list of active Keys objects.
    struct _keypad_keys_obj_t *next;
} keypad_keys_obj_t;

void keypad_keys_tick(void);
void keypad_keys_reset(void);

#endif  // MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_KEYS_H
