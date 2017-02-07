/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#ifndef __MICROPY_INCLUDED_ESP8266_XTIRQ_H__
#define __MICROPY_INCLUDED_ESP8266_XTIRQ_H__

#include <stdint.h>

// returns the value of "intlevel" from the PS register
static inline uint32_t query_irq(void) {
    uint32_t ps;
    __asm__ volatile("rsr %0, ps" : "=a" (ps));
    return ps & 0xf;
}

// irqs with a priority value lower or equal to "intlevel" will be disabled
// "intlevel" should be between 0 and 15 inclusive, and should be an integer
static inline uint32_t raise_irq_pri(uint32_t intlevel) {
    uint32_t old_ps;
    __asm__ volatile ("rsil %0, %1" : "=a" (old_ps) : "I" (intlevel));
    return old_ps;
}

// "ps" should be the value returned from raise_irq_pri
static inline void restore_irq_pri(uint32_t ps) {
    __asm__ volatile ("wsr %0, ps; rsync" :: "a" (ps));
}

static inline uint32_t disable_irq(void) {
    return raise_irq_pri(15);
}

static inline void enable_irq(uint32_t irq_state) {
    restore_irq_pri(irq_state);
}

#endif // __MICROPY_INCLUDED_ESP8266_XTIRQ_H__
