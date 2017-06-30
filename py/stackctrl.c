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

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

void mp_stack_ctrl_init(void) {
    volatile int stack_dummy;
    MP_STATE_THREAD(stack_top) = (char*)&stack_dummy;
}

void mp_stack_set_top(void *top) {
    MP_STATE_THREAD(stack_top) = top;
}

mp_uint_t mp_stack_usage(void) {
    // Assumes descending stack
    volatile int stack_dummy;
    return MP_STATE_THREAD(stack_top) - (char*)&stack_dummy;
}

#if MICROPY_STACK_CHECK

void mp_stack_set_limit(mp_uint_t limit) {
    MP_STATE_THREAD(stack_limit) = limit;
}

void mp_exc_recursion_depth(void) {
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_RuntimeError,
        MP_OBJ_NEW_QSTR(MP_QSTR_maximum_space_recursion_space_depth_space_exceeded)));
}

void mp_stack_check(void) {
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        mp_exc_recursion_depth();
    }
}

#endif // MICROPY_STACK_CHECK
