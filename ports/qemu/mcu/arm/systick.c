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

// CPU frequency
#ifndef CPU_FREQ_HZ
#define CPU_FREQ_HZ             25000000u
#endif

// SysTick registers
#define SYSTICK_BASE            0xE000E010
#define SYSTICK_CSR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_RVR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_CVR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

// SysTick config bits
#define SYSTICK_CSR_ENABLE      (1 << 0)
#define SYSTICK_CSR_TICKINT     (1 << 1)
#define SYSTICK_CSR_CLKSOURCE   (1 << 2)

static volatile uint32_t ticks_ms = 0;

void systick_init(void) {
    // Configure SysTick: reload at 1ms intervals
    SYSTICK_CVR = 0;
    SYSTICK_RVR = (CPU_FREQ_HZ / 1000) - 1;
    SYSTICK_CSR = SYSTICK_CSR_ENABLE | SYSTICK_CSR_TICKINT | SYSTICK_CSR_CLKSOURCE;
}

uintptr_t systick_ticks_ms(void) {
    return ticks_ms;
}

uintptr_t systick_ticks_us(void) {
    // Get current values atomically
    uint32_t ms = ticks_ms;
    uint32_t cvr = SYSTICK_CVR;

    // Calculate microseconds from the down-counter
    uint32_t elapsed_cycles = SYSTICK_RVR - cvr;
    uint32_t us_fraction = (elapsed_cycles * 1000) / (CPU_FREQ_HZ / 1000);

    return (ms * 1000) + us_fraction;
}

void SysTick_Handler(void) {
    ticks_ms++;
}
