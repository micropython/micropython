/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// Wall-clock time for the time module. Included into extmod/modtime.c via
// MICROPY_PY_TIME_INCLUDEFILE (not compiled standalone). Provides time(), time_ns(),
// and gmtime/localtime/mktime via shared/timeutils. All three views come from the one
// anchored wall clock (mp_uefi_wall_ns, uefi_time.c): the RTC seeds the anchor and the
// counter interpolates, so seconds and sub-seconds are always mutually coherent.

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"
#include "uefi_port.h"

static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    timeutils_seconds_since_epoch_to_struct_time((mp_timestamp_t)(mp_uefi_wall_ns() / 1000000000ULL), tm);
}

static mp_obj_t mp_time_time_get(void) {
    return mp_obj_new_int_from_ull((unsigned long long)(mp_uefi_wall_ns() / 1000000000ULL));
}

uint64_t mp_hal_time_ns(void) {
    return mp_uefi_wall_ns();
}
