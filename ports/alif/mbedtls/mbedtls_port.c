/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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

#include "py/obj.h"
#include "se_services.h"
#include "mbedtls_config_port.h"

#if defined(MBEDTLS_HAVE_TIME)
#include "shared/timeutils/timeutils.h"
#include "mbedtls/platform_time.h"
#endif

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {
    uint32_t val = 0;
    int n = 0;
    *olen = len;
    while (len--) {
        if (!n) {
            val = se_services_rand64();
            n = 4;
        }
        *output++ = val;
        val >>= 8;
        --n;
    }
    return 0;
}

#if defined(MBEDTLS_HAVE_TIME)

time_t alif_mbedtls_time(time_t *timer) {
    // TODO implement proper RTC time
    unsigned int year = 2025;
    unsigned int month = 1;
    unsigned int date = 1;
    unsigned int hours = 12;
    unsigned int minutes = 0;
    unsigned int seconds = 0;
    return timeutils_seconds_since_epoch(year, month, date, hours, minutes, seconds);
}

#endif
