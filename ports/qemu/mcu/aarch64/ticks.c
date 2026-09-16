/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython Contributors
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

#ifndef CNTFRQ_HZ
#define CNTFRQ_HZ             10000000u
#endif

static uint64_t ticks_get_ticks(void) {
    uint64_t ticks;
    __asm__ volatile (
        "mrs %0, cntpct_el0\n"
        : "=r" (ticks)
        :
        : "memory"
        );
    return ticks;
}

static uint32_t ticks_get_freq(void) {
    uint64_t freq;
    __asm__ volatile (
        "mrs %0, cntfrq_el0\n"
        : "=r" (freq)
        );
    return freq ? (uint32_t)freq : CNTFRQ_HZ;
}

// The counter frequency is fixed at boot, so the derived divisors are computed
// once.  They are clamped to at least 1 because a counter running slower than
// the requested resolution would otherwise divide by zero, and because 0 is
// also the "not initialised yet" sentinel.
static uint32_t ticks_divisor(uint32_t hz) {
    return hz == 0 ? 1 : hz;
}

uintptr_t ticks_ms(void) {
    static uint32_t per_ms;
    if (per_ms == 0) {
        per_ms = ticks_divisor(ticks_get_freq() / 1000);
    }
    return (uintptr_t)(ticks_get_ticks() / per_ms);
}

uintptr_t ticks_us(void) {
    static uint32_t per_us;
    if (per_us == 0) {
        per_us = ticks_divisor(ticks_get_freq() / 1000000);
    }
    return (uintptr_t)(ticks_get_ticks() / per_us);
}
