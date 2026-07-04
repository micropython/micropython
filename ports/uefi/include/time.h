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

// Minimal <time.h> shim for the freestanding UEFI port. Provides just the types
// mbedTLS needs (time_t / struct tm); the actual clock comes from the UEFI RTC via
// mbedtls_uefi_time / mbedtls_platform_gmtime_r in uefi_mbedtls_port.c.
#ifndef _UEFI_LIBC_TIME_H
#define _UEFI_LIBC_TIME_H

#include <stddef.h>

typedef long long time_t;

struct tm {
    int tm_sec;     // 0..60
    int tm_min;     // 0..59
    int tm_hour;    // 0..23
    int tm_mday;    // 1..31
    int tm_mon;     // 0..11
    int tm_year;    // years since 1900
    int tm_wday;    // 0..6, 0 = Sunday
    int tm_yday;    // 0..365
    int tm_isdst;   // daylight saving flag
};

#endif // _UEFI_LIBC_TIME_H
