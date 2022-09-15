/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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

#include "mphalport.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "lib/oofatfs/ff.h"

extern uint32_t time_offset;

MP_WEAK DWORD get_fattime(void) {
    #if MICROPY_PY_MACHINE_RTC
    return (RTC->MODE2.CLOCK.reg >> 1) + (20 << 25);
    #else
    extern void rtc_gettime(timeutils_struct_time_t *tm);
    timeutils_struct_time_t tm;
    timeutils_seconds_since_epoch_to_struct_time(mp_hal_ticks_ms_64() / 1000 + time_offset, &tm);
    return ((tm.tm_year - 1980) << 25) | ((tm.tm_mon) << 21) | ((tm.tm_mday) << 16) |
           ((tm.tm_hour) << 11) | ((tm.tm_min) << 5) | (tm.tm_sec / 2);
    #endif
}
