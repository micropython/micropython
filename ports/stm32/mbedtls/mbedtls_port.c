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

#include "rng.h"
#include "mbedtls_config.h"
#include "rtc.h"
#include "shared/timeutils/timeutils.h"

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
    uint32_t val = 0;
    int n = 0;
    *olen = len;
    while (len--) {
        if (!n) {
            val = rng_get();
            n = 4;
        }
        *output++ = val;
        val >>= 8;
        --n;
    }
    return 0;
}

time_t platform_mbedtls_time(time_t *timer) {
    // mbedtls_time requires time in seconds from EPOCH 1970
    rtc_init_finalise();
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
    return timeutils_seconds_since_epoch(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds) + TIMEUTILS_SECONDS_1970_TO_2000;
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
