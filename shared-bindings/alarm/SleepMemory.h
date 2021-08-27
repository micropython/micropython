/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_SLEEPMEMORY_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_SLEEPMEMORY_H

#include "common-hal/alarm/SleepMemory.h"

extern const mp_obj_type_t alarm_sleep_memory_type;

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self);

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len);
void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_SLEEPMEMORY_H
