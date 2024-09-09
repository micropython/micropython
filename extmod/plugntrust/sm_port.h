/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
#ifndef __SM_PORT_H__
#define __SM_PORT_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#define SM_MUTEX_DEFINE(x)
#define SM_MUTEX_INIT(x)
#define SM_MUTEX_DEINIT(x)
#define SM_MUTEX_LOCK(x)
#define SM_MUTEX_UNLOCK(x)

#ifndef SMLOG_DEBUG_MESSAGES
#define SMLOG_I(...)
#define SMLOG_E(...)
#define SMLOG_W(...)
#define SMLOG_D(...)
#define SMLOG_AU8_D(BUF, LEN)
#define SMLOG_MAU8_D(MSG, BUF, LEN)
#else
#define DEBUG_PRINTF(...)   printf(__VA_ARGS__)
#define SMLOG_I(...)        DEBUG_PRINTF(__VA_ARGS__)
#define SMLOG_E(...)        DEBUG_PRINTF(__VA_ARGS__)
#define SMLOG_W(...)        DEBUG_PRINTF(__VA_ARGS__)
#define SMLOG_D(...)        DEBUG_PRINTF(__VA_ARGS__)

#define SMLOG_AU8_D(BUF, LEN)                               \
    DEBUG_PRINTF(" :");                                     \
    for (size_t bufIndex = 0; bufIndex < LEN; bufIndex++) { \
        DEBUG_PRINTF("%x", BUF[bufIndex]);                  \
    }                                                       \
    DEBUG_PRINTF("\n")

#define SMLOG_MAU8_D(MSG, BUF, LEN)                         \
    DEBUG_PRINTF(MSG);                                      \
    DEBUG_PRINTF(" :");                                     \
    for (size_t bufIndex = 0; bufIndex < LEN; bufIndex++) { \
        DEBUG_PRINTF("%x", BUF[bufIndex]);                  \
    }                                                       \
    DEBUG_PRINTF("\n")
#endif

void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr);
#define sm_malloc(x) m_tracked_calloc(1, x)
#define sm_free m_tracked_free

#endif // #ifndef __SM_PORT_H__
