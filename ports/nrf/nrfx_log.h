/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
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

#ifndef NRFX_LOG_H
#define NRFX_LOG_H

#include <stdio.h>
#include "mphalport.h"
#include "nrfx_config.h"

#define LOG_TEST_UART 1

#define TEST_MODULE_IMPL(x, y) LOG_TEST_ ## x == LOG_TEST_ ## y
#define TEST_MODULE(x, y) TEST_MODULE_IMPL(x, y)

#if (!defined(NRFX_LOG_ENABLED) || (NRFX_LOG_ENABLED == 0)) || \
    (TEST_MODULE(NRFX_LOG_MODULE, UART) && defined(NRFX_LOG_UART_DISABLED) && NRFX_LOG_UART_DISABLED)

    #define NRFX_LOG_DEBUG(fmt, ...)
    #define NRFX_LOG_ERROR(fmt, ...)
    #define NRFX_LOG_WARNING(fmt, ...)
    #define NRFX_LOG_INFO(fmt, ...)

    #define NRFX_LOG_HEXDUMP_ERROR(p_memory, length)
    #define NRFX_LOG_HEXDUMP_WARNING(p_memory, length)
    #define NRFX_LOG_HEXDUMP_INFO(p_memory, length)
    #define NRFX_LOG_HEXDUMP_DEBUG(p_memory, length)
    #define NRFX_LOG_ERROR_STRING_GET(error_code) ""

#else

    #define VALUE_TO_STR(x) #x
    #define VALUE(x) VALUE_TO_STR(x)

    #define LOG_PRINTF(fmt, ...) \
        do { \
            printf("%s: ", VALUE(NRFX_LOG_MODULE)); \
            printf(fmt, ##__VA_ARGS__); \
            printf("\n"); \
        } while (0)

    #define NRFX_LOG_DEBUG   LOG_PRINTF
    #define NRFX_LOG_ERROR   LOG_PRINTF
    #define NRFX_LOG_WARNING LOG_PRINTF
    #define NRFX_LOG_INFO    LOG_PRINTF


    #define NRFX_LOG_HEXDUMP_ERROR(p_memory, length)

    #define NRFX_LOG_HEXDUMP_WARNING(p_memory, length)

    #define NRFX_LOG_HEXDUMP_INFO(p_memory, length)

    #define NRFX_LOG_HEXDUMP_DEBUG(p_memory, length)

    #define NRFX_LOG_ERROR_STRING_GET(error_code) \
        nrfx_error_code_lookup(error_code)

#endif // NRFX_LOG_ENABLED

#endif // NRFX_LOG_H
