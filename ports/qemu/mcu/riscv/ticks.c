/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

// RISC-V timebase frequency for QEMU
#ifndef TIMEBASE_FREQ_HZ
#define TIMEBASE_FREQ_HZ        10000000u
#endif

static uint64_t ticks_get_ticks(void) {
    #if __riscv_xlen < 64
    uint32_t ticks_lo = 0;
    uint32_t ticks_hi = 0;
    uint32_t rollover = 0;
    do {
        __asm volatile (
            "rdtimeh %0 \n"
            "rdtime  %1 \n"
            "rdtimeh %2 \n"
            : "=r" (ticks_hi), "=r" (ticks_lo), "=r" (rollover)
            :
            :
            );
    } while (ticks_hi != rollover);
    return ((uint64_t)(ticks_hi) << 32ULL) | (uint64_t)(ticks_lo);
    #else
    uint64_t ticks = 0;
    __asm volatile (
        "rdtime %0 \n"
        : "=r" (ticks)
        :
        :
        );
    return ticks;
    #endif
}

uintptr_t ticks_ms(void) {
    return (uintptr_t)(ticks_get_ticks() / (TIMEBASE_FREQ_HZ / 1000));
}

uintptr_t ticks_us(void) {
    return (uintptr_t)(ticks_get_ticks() / (TIMEBASE_FREQ_HZ / 1000000));
}
