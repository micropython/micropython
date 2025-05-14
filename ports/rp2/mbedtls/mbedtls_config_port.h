/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_MBEDTLS_CONFIG_H
#define MICROPY_INCLUDED_MBEDTLS_CONFIG_H

// Set mbedtls configuration
#define MBEDTLS_ECP_NIST_OPTIM

// Time hook
#include <time.h>
time_t rp2_rtctime_seconds(time_t *timer);
#define MBEDTLS_PLATFORM_TIME_MACRO rp2_rtctime_seconds
#define MBEDTLS_PLATFORM_MS_TIME_ALT mbedtls_ms_time

// Set MicroPython-specific options.
#define MICROPY_MBEDTLS_CONFIG_BARE_METAL (1)

// Include common mbedtls configuration.
#include "extmod/mbedtls/mbedtls_config_common.h"

#endif /* MICROPY_INCLUDED_MBEDTLS_CONFIG_H */
