/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_TIME___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_TIME___INIT___H

#include <stdint.h>
#include <stdbool.h>

#include "lib/timeutils/timeutils.h"

extern mp_obj_t struct_time_from_tm(timeutils_struct_time_t *tm);
extern void struct_time_to_tm(mp_obj_t t, timeutils_struct_time_t *tm);

extern uint64_t common_hal_time_monotonic_ms(void);
extern uint64_t common_hal_time_monotonic_ns(void);
extern void common_hal_time_delay_ms(uint32_t);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_TIME___INIT___H
