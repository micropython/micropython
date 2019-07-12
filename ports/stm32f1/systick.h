/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_SYSTICK_H
#define MICROPY_INCLUDED_STM32_SYSTICK_H

// Works for x between 0 and 16 inclusive
#define POW2_CEIL(x) ((((x) - 1) | ((x) - 1) >> 1 | ((x) - 1) >> 2 | ((x) - 1) >> 3) + 1)

enum {
    SYSTICK_DISPATCH_DMA = 0,
    #if MICROPY_HW_ENABLE_STORAGE
    SYSTICK_DISPATCH_STORAGE,
    #endif
    #if MICROPY_PY_NETWORK && MICROPY_PY_LWIP
    SYSTICK_DISPATCH_LWIP,
    #endif
    SYSTICK_DISPATCH_MAX
};

#define SYSTICK_DISPATCH_NUM_SLOTS POW2_CEIL(SYSTICK_DISPATCH_MAX)

typedef void (*systick_dispatch_t)(uint32_t);

extern systick_dispatch_t systick_dispatch_table[SYSTICK_DISPATCH_NUM_SLOTS];

static inline void systick_enable_dispatch(size_t slot, systick_dispatch_t f) {
    systick_dispatch_table[slot] = f;
}

static inline void systick_disable_dispatch(size_t slot) {
    systick_dispatch_table[slot] = NULL;
}

void systick_wait_at_least(uint32_t stc, uint32_t delay_ms);
bool systick_has_passed(uint32_t stc, uint32_t delay_ms);

#endif // MICROPY_INCLUDED_STM32_SYSTICK_H
