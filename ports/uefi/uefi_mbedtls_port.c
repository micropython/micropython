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

// mbedTLS platform hooks for the UEFI port (built only when TLS=mbedtls).
//
// The bare-metal mbedtls config (mbedtls/mbedtls_config_port.h) disables the
// platform's own time, so we supply the wall/monotonic clocks here:
//   * mbedtls_uefi_time         — wall clock (Unix epoch) from the UEFI RTC
//   * mbedtls_platform_gmtime_r — break a time_t into struct tm for X.509 validity
//   * mbedtls_ms_time           — a monotonic millisecond counter
// (Entropy, mbedtls_hardware_poll, is already provided by extmod/mbedtls/mbedtls_alt.c
// from mp_hal_get_random, so it is deliberately NOT defined here.)

#include <stddef.h>
#include <time.h>

#include "py/mpconfig.h"

#if MICROPY_PY_SSL && MICROPY_SSL_MBEDTLS

#include "shared/timeutils/timeutils.h"
#include "efi.h"
#include "uefi_port.h"
#include "mphalport.h"
#include "mbedtls/platform_util.h"   // mbedtls_ms_time_t

// Seconds since the Unix epoch from the platform RTC (the port builds with
// MICROPY_EPOCH_IS_1970, so timeutils' epoch matches Unix time). 0 if unavailable.
static time_t uefi_unix_time(void) {
    EFI_TIME t;
    if (mp_uefi_st->RuntimeServices == NULL ||
        EFI_ERROR(mp_uefi_st->RuntimeServices->GetTime(&t, NULL))) {
        return 0;
    }
    return (time_t)timeutils_seconds_since_epoch(t.Year, t.Month, t.Day,
        t.Hour, t.Minute, t.Second);
}

time_t mbedtls_uefi_time(time_t *timer) {
    time_t now = uefi_unix_time();
    if (timer != NULL) {
        *timer = now;
    }
    return now;
}

// MBEDTLS_PLATFORM_MS_TIME_ALT: a monotonic millisecond counter (session tickets,
// DTLS retransmit timers). The port's tick clock is exactly this.
mbedtls_ms_time_t mbedtls_ms_time(void) {
    return (mbedtls_ms_time_t)mp_hal_ticks_ms();
}

// MBEDTLS_PLATFORM_GMTIME_R_ALT: convert a Unix time_t to a UTC struct tm using
// shared/timeutils. mbedtls only reads y/m/d/h/m/s for certificate validity, but
// we fill wday/yday too (converting timeutils' Mon=0 / 1-based yday conventions).
struct tm *mbedtls_platform_gmtime_r(const time_t *timer, struct tm *tm_buf) {
    if (timer == NULL || tm_buf == NULL) {
        return NULL;
    }
    timeutils_struct_time_t st;
    timeutils_seconds_since_epoch_to_struct_time((mp_timestamp_t)*timer, &st);
    tm_buf->tm_year = st.tm_year - 1900;
    tm_buf->tm_mon = st.tm_mon - 1;
    tm_buf->tm_mday = st.tm_mday;
    tm_buf->tm_hour = st.tm_hour;
    tm_buf->tm_min = st.tm_min;
    tm_buf->tm_sec = st.tm_sec;
    tm_buf->tm_wday = (st.tm_wday + 1) % 7;   // timeutils Mon=0 -> struct tm Sun=0
    tm_buf->tm_yday = st.tm_yday - 1;         // timeutils 1..366 -> struct tm 0..365
    tm_buf->tm_isdst = 0;
    return tm_buf;
}

#endif // MICROPY_PY_SSL && MICROPY_SSL_MBEDTLS
