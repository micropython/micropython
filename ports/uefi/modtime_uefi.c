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

// Wall-clock time for the time module, from RuntimeServices->GetTime (the
// platform RTC). Included into extmod/modtime.c via MICROPY_PY_TIME_INCLUDEFILE
// (not compiled standalone). Provides time(), time_ns(), and gmtime/localtime/
// mktime via shared/timeutils.

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"
#include "efi.h"
#include "uefi_port.h"

// Seconds since the MicroPython epoch from the RTC; *nanos_out gets the sub-second
// part. Returns 0 if the RTC is unavailable.
static mp_timestamp_t uefi_rtc_seconds(uint32_t *nanos_out) {
    EFI_TIME t;
    if (mp_uefi_st->RuntimeServices == NULL ||
        EFI_ERROR(mp_uefi_st->RuntimeServices->GetTime(&t, NULL))) {
        if (nanos_out != NULL) {
            *nanos_out = 0;
        }
        return 0;
    }
    if (nanos_out != NULL) {
        *nanos_out = t.Nanosecond;
    }
    return timeutils_seconds_since_epoch(t.Year, t.Month, t.Day, t.Hour, t.Minute, t.Second);
}

static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    timeutils_seconds_since_epoch_to_struct_time(uefi_rtc_seconds(NULL), tm);
}

static mp_obj_t mp_time_time_get(void) {
    return mp_obj_new_int_from_ull((unsigned long long)uefi_rtc_seconds(NULL));
}

uint64_t mp_hal_time_ns(void) {
    uint32_t ns;
    mp_timestamp_t s = uefi_rtc_seconds(&ns);
    return (uint64_t)timeutils_seconds_since_epoch_to_nanoseconds_since_1970(s) + ns;
}
