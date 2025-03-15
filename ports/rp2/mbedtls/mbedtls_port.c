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
#include <py/mpconfig.h>

#ifdef MICROPY_SSL_MBEDTLS

#include "mbedtls_config_port.h"

#include "shared/timeutils/timeutils.h"
#include "mbedtls/platform_time.h"
#include "pico/aon_timer.h"

extern uint8_t rosc_random_u8(size_t cycles);

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {
    *olen = len;
    for (size_t i = 0; i < len; i++) {
        output[i] = rosc_random_u8(8);
    }
    return 0;
}

time_t rp2_rtctime_seconds(time_t *timer) {
    struct timespec ts;
    aon_timer_get_time(&ts);
    return ts.tv_sec;
}

mbedtls_ms_time_t mbedtls_ms_time(void) {
    time_t *tv = NULL;
    mbedtls_ms_time_t current_ms;
    current_ms = rp2_rtctime_seconds(tv) * 1000;
    return current_ms;
}
#endif
