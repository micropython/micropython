/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
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

#include "py/runtime.h"
#include "py/stackctrl.h"

void mp_stack_ctrl_init(void) {
    // Force routine to not be inlined. Better guarantee than MP_NOINLINE for -flto.
    __asm volatile ("");
    volatile int stack_dummy;
    MP_STATE_THREAD(stack_top) = (char *)&stack_dummy;
}

void mp_stack_set_top(void *top) {
    MP_STATE_THREAD(stack_top) = top;
}

mp_uint_t mp_stack_usage(void) {
    // Assumes descending stack
    // Force routine to not be inlined. Better guarantee than MP_NOINLINE for -flto.
    __asm volatile ("");
    volatile int stack_dummy;
    return MP_STATE_THREAD(stack_top) - (char *)&stack_dummy;
}

#if MICROPY_STACK_CHECK

void mp_stack_set_limit(mp_uint_t limit) {
    MP_STATE_THREAD(stack_limit) = limit;
}

void mp_stack_check(void) {
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        mp_raise_recursion_depth();
    }
}

#endif // MICROPY_STACK_CHECK

#if MICROPY_MAX_STACK_USAGE

// Fill stack space with this unusual value.
const char MP_MAX_STACK_USAGE_SENTINEL_BYTE = 0xEE;

// Record absolute bottom (logical limit) of stack.
void mp_stack_set_bottom(void *stack_bottom) {
    MP_STATE_THREAD(stack_bottom) = stack_bottom;
}

// Return the current frame pointer. This can be used as an
// approximation for the stack pointer of the _calling_ function.
// This routine must not be inlined.  This method is
// architecture-independent, as opposed to using asm("sp") or similar.
//
// The stack_dummy approach used elsewhere in this file is not safe in
// all cases. That value may be below the actual top of the stack.
static void *approx_stack_pointer(void) {
    __asm volatile ("");
    return __builtin_frame_address(0);
}

// Fill stack space down toward the stack limit with a known unusual value.
void mp_stack_fill_with_sentinel(void) {
    // Force routine to not be inlined. Better guarantee than MP_NOINLINE for -flto.
    __asm volatile ("");
    // Start filling stack just below the current stack frame.
    // Continue until we've hit the bottom of the stack (lowest address,
    // logical "ceiling" of stack).
    char *p = (char *)approx_stack_pointer() - 1;

    while (p >= MP_STATE_THREAD(stack_bottom)) {
        *p-- = MP_MAX_STACK_USAGE_SENTINEL_BYTE;
    }
}

#endif // MICROPY_MAX_STACK_USAGE
