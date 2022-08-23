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

#include "py/runtime.h"
#include "fsl_trng.h"
#include "mbedtls_config.h"
#include "shared/timeutils/timeutils.h"
#include "extmod/utime_mphal.h"
#include "fsl_snvs_lp.h"

struct tm {
    uint16_t tm_year;        // i.e. 2014
    uint8_t tm_mon;          // 1..12
    uint8_t tm_mday;         // 1..31
    uint8_t tm_hour;         // 0..23
    uint8_t tm_min;          // 0..59
    uint8_t tm_sec;          // 0..59
    uint8_t tm_wday;         // 0..6  0 = Monday
    uint16_t tm_yday;        // 1..366
};

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {

    // assumes that TRNG_Init was called during startup
    *olen = len;
    TRNG_GetRandomData(TRNG, output, len);

    return 0;
}


time_t platform_mbedtls_time(time_t *timer) {
    // mbedtls_time requires time in seconds from EPOCH 1970
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    // EPOCH is 1970 for this port, which leads to the following trouble:
    // timeutils_seconds_since_epoch() calls timeutils_seconds_since_2000(), and
    // timeutils_seconds_since_2000() subtracts 2000 from year, but uses
    // an unsigned number for seconds, That causes an underrun, which is not
    // fixed by adding the TIMEUTILS_SECONDS_1970_TO_2000.
    // Masking it to 32 bit for year < 2000 fixes it.
    return timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.minute, t.second)
           & (t.year < 2000 ? 0xffffffff : 0xffffffffffff);
}


struct tm *mbedtls_platform_gmtime_r(const mbedtls_time_t *tt, struct tm *tm_buf) {
    /**
     * \param tt     Pointer to an object containing time (in seconds) since the
     *               epoch to be converted
     * \param tm_buf Pointer to an object where the results will be stored
     *
     * \return      Pointer to an object of type struct tm on success, otherwise
     *              NULL
     */
    int seconds = *tt;
    timeutils_struct_time_t tmd;
    timeutils_seconds_since_epoch_to_struct_time(seconds, &tmd);
    tm_buf->tm_year = tmd.tm_year;
    tm_buf->tm_mon = tmd.tm_mon;
    tm_buf->tm_mday = tmd.tm_mday;
    tm_buf->tm_hour = tmd.tm_hour;
    tm_buf->tm_min = tmd.tm_min;
    tm_buf->tm_sec = tmd.tm_sec;
    tm_buf->tm_wday = tmd.tm_wday;
    tm_buf->tm_yday = tmd.tm_yday;


    return tm_buf;
}

#endif
