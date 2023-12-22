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

// If you want to debug MBEDTLS uncomment the following and
// Pass 3 to mbedtls_debug_set_threshold in socket_new
// #define MBEDTLS_DEBUG_C

// Set mbedtls configuration
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_DEPRECATED_REMOVED
#define MBEDTLS_ENTROPY_HARDWARE_ALT

// Enable mbedtls modules
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C
#undef MBEDTLS_HAVE_TIME_DATE

// Memory allocation hooks
#include <stdlib.h>
#include <stdio.h>
void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr);
#define MBEDTLS_PLATFORM_STD_CALLOC m_tracked_calloc
#define MBEDTLS_PLATFORM_STD_FREE m_tracked_free
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf

#include "mbedtls/check_config.h"

#endif /* MICROPY_INCLUDED_MBEDTLS_CONFIG_H */
