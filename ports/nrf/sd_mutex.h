/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_NRF_SD_MUTEX_H
#define MICROPY_INCLUDED_NRF_SD_MUTEX_H

#include "nrf_soc.h"

// Helpers for common usage of nrf_mutex.

// Try to acquire a mutex right now. Raise exception if we can't get it.
void sd_mutex_acquire_check(nrf_mutex_t *p_mutex);

// Wait for a mutex to become available. Run VM background tasks while waiting.
void sd_mutex_acquire_wait(nrf_mutex_t *p_mutex);

// Wait for a mutex to become available.. Block VM while waiting.
void sd_mutex_acquire_wait_no_vm(nrf_mutex_t *p_mutex);

// Release a mutex, and raise exception on error.
void sd_mutex_release_check(nrf_mutex_t *p_mutex);

#endif // MICROPY_INCLUDED_NRF_SD_MUTEX_H
