/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include "py/mphal.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"        /* FatFs lower layer API */
#include "lib/oofatfs/diskio.h"    /* FatFs lower layer API */
#include "lib/timeutils/timeutils.h"

#if CIRCUITPY_RTC
#include "shared-bindings/rtc/RTC.h"
#endif

DWORD get_fattime(void) {
    #if CIRCUITPY_RTC
    timeutils_struct_time_t tm;
    common_hal_rtc_get_time(&tm);
    return ((tm.tm_year - 1980) << 25) | (tm.tm_mon << 21) | (tm.tm_mday << 16) |
           (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
    #else
    return ((2016 - 1980) << 25) | ((9) << 21) | ((1) << 16) | ((16) << 11) | ((43) << 5) | (35 / 2);
    #endif
}
