/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2024 Angus Gratton
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
#ifndef MICROPY_INCLUDED_PY_CSTACK_H
#define MICROPY_INCLUDED_PY_CSTACK_H

#include "py/mpstate.h"

// Both init functions below accept the full stack size. Set the
// MICROPY_STACK_CHECK_MARGIN to the number of bytes subtracted to account
// for stack usage between checks.

void mp_cstack_init_with_sp_here(size_t stack_size);

static inline void mp_cstack_init_with_top(void *top, size_t stack_size) {
    MP_STATE_THREAD(stack_top) = (char *)top;

    #if MICROPY_STACK_CHECK
    assert(stack_size > MICROPY_STACK_CHECK_MARGIN); // Should be enforced by port
    MP_STATE_THREAD(stack_limit) = stack_size - MICROPY_STACK_CHECK_MARGIN;
    #else
    (void)stack_size;
    #endif
}

mp_uint_t mp_cstack_usage(void);

#if MICROPY_STACK_CHECK

void mp_cstack_check(void);

#else

static inline void mp_cstack_check(void) {
    // No-op when stack checking is disabled
}

#endif

#endif // MICROPY_INCLUDED_PY_CSTACK_H
