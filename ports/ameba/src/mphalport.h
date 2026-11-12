/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Realtek Corporation
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

#ifndef MICROPY_INCLUDED_AMEBA_MPHALPORT_H
#define MICROPY_INCLUDED_AMEBA_MPHALPORT_H

#include <stdint.h>

// Save PRIMASK and disable all interrupts; return saved state for restoration.
static inline uint32_t mp_hal_begin_atomic_section(void) {
    uint32_t state;
    __asm volatile ("mrs %0, primask\n\t cpsid i" : "=r" (state) :: "memory");
    return state;
}

// Restore PRIMASK to the value saved by mp_hal_begin_atomic_section().
static inline void mp_hal_end_atomic_section(uint32_t state) {
    __asm volatile ("msr primask, %0" :: "r" (state) : "memory");
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     mp_hal_begin_atomic_section()
#define MICROPY_END_ATOMIC_SECTION(state)  mp_hal_end_atomic_section(state)

mp_uint_t mp_hal_ticks_us(void);
void mp_hal_set_interrupt_char(int c);
void mp_hal_get_random(size_t n, void *buf);

#endif // MICROPY_INCLUDED_AMEBA_MPHALPORT_H
