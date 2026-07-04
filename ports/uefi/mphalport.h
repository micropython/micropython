/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// HAL declarations for the UEFI port. Core stdout/stdin/cooked functions are
// declared by py/mphal.h and implemented in mphalport.c / shared stdout_helpers.c.
// The timebase (mp_hal_ticks_*) and yielding delays live in uefi_time.c; stdin
// and mp_hal_set_interrupt_char live in uefi_event.c.

#ifndef MICROPY_INCLUDED_UEFI_MPHALPORT_H
#define MICROPY_INCLUDED_UEFI_MPHALPORT_H

#include "py/mpconfig.h"

// Monotonic timebase backed by a hardware counter (CNTVCT_EL0 / TSC).
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_cpu(void);

void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);

// readline cursor control without VT100 escapes (MICROPY_HAL_HAS_VT100 == 0),
// implemented over ConOut-safe backspace/space output.
void mp_hal_move_cursor_back(unsigned int pos);
void mp_hal_erase_line_from_cursor(unsigned int n_chars_to_erase);

void mp_hal_set_interrupt_char(int c);

// Entropy for os.urandom() (uefi_rng.c): EFI_RNG -> RDRAND -> counter PRNG.
void mp_hal_get_random(size_t n, void *buf);

#endif // MICROPY_INCLUDED_UEFI_MPHALPORT_H
