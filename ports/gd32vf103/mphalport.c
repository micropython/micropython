/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Emil Renner Berthing
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

#include <stdint.h>
#include <stddef.h>

#include "py/mphal.h"

#include "lib/mtimer.h"

/* sanity check */
#if MICROPY_PY_UTIME_TICKS_PERIOD != (1U << 30)
#error This implementation is only correct for MICROPY_PY_UTIME_TICKS_PERIOD == 2^30
#endif

void mp_hal_delay_ms(mp_uint_t ms) {
    mtimer_delay(ms * (MTIMER_FREQ / 1000));
}

void mp_hal_delay_us(mp_uint_t us) {
    mtimer_delay(us * (MTIMER_FREQ / 1000000));
}

mp_uint_t mp_hal_ticks_ms(void) {
    uint64_t mtime = mtimer_mtime();

    /* return (mtime / (CORECLOCK/4000)) % 2^30
     * using a / b = a * (2^34 / b) / 2^34
     */
    mtime *= ((4000ULL << 34) - 1) / CORECLOCK + 1;
    return mtime >> 34;
}

mp_uint_t mp_hal_ticks_us(void) {
    uint64_t mtime = mtimer_mtime();

    /* return (mtime / (CORECLOCK/4000000)) % 2^30
     * using a / b = a * (2^34 / b) / 2^34
     */
    mtime *= ((4000000ULL << 34) - 1) / CORECLOCK + 1;
    return mtime >> 34;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return MTIMER->mtime_lo % MICROPY_PY_UTIME_TICKS_PERIOD;
}

/* vim: set ts=4 sw=4 et: */
