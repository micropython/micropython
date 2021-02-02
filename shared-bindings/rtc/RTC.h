/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_RTC_RTC_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_RTC_RTC_H

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "lib/timeutils/timeutils.h"

extern void common_hal_rtc_get_time(timeutils_struct_time_t *tm);
extern void common_hal_rtc_set_time(timeutils_struct_time_t *tm);

extern int common_hal_rtc_get_calibration(void);
extern void common_hal_rtc_set_calibration(int calibration);

extern const mp_obj_type_t rtc_rtc_type;

typedef struct _rtc_rtc_obj_t {
    mp_obj_base_t base;
} rtc_rtc_obj_t;

extern const rtc_rtc_obj_t rtc_rtc_obj;

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_RTC_RTC_H
