/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#ifdef MICROPY_SSL_MBEDTLS

#include "mbedtls_config_port.h"
#include <stdint.h>
uint32_t trng_random_u32(void);
#if defined(MBEDTLS_HAVE_TIME) || defined(MBEDTLS_HAVE_TIME_DATE)
#include <stdbool.h>
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "mbedtls/platform_time.h"
extern void rtc_gettime(timeutils_struct_time_t *tm);
#endif

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {

    *olen = len;
    for (size_t i = 0; i < len; i++) {
        output[i] = (unsigned char)trng_random_u32();
    }
    return 0;
}

#endif

#if defined(MBEDTLS_HAVE_TIME)
time_t samd_rtctime_seconds(time_t *timer) {
    timeutils_struct_time_t tm;
    rtc_gettime(&tm);
    return timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

mbedtls_ms_time_t mbedtls_ms_time(void) {
    time_t *tv = NULL;
    mbedtls_ms_time_t current_ms;
    current_ms = samd_rtctime_seconds(tv) * 1000;
    return current_ms;
}
#endif

#if defined(MBEDTLS_HAVE_TIME_DATE)
struct tm *gmtime(const time_t *timep) {
    static struct tm tm;
    timeutils_struct_time_t tm_buf = {0};
    timeutils_seconds_since_epoch_to_struct_time(*timep, &tm_buf);

    tm.tm_sec = tm_buf.tm_sec;
    tm.tm_min = tm_buf.tm_min;
    tm.tm_hour = tm_buf.tm_hour;
    tm.tm_mday = tm_buf.tm_mday;
    tm.tm_mon = tm_buf.tm_mon - 1;
    tm.tm_year = tm_buf.tm_year - 1900;
    tm.tm_wday = tm_buf.tm_wday;
    tm.tm_yday = tm_buf.tm_yday;
    tm.tm_isdst = -1;

    return &tm;
}
#endif
