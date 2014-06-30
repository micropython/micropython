/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "mpconfig.h"
#include "misc.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "stackctrl.h"

// Stack top at the start of program
char *stack_top;

void mp_stack_ctrl_init() {
    volatile int stack_dummy;
    stack_top = (char*)&stack_dummy;
}

uint mp_stack_usage() {
    // Assumes descending stack
    volatile int stack_dummy;
    return stack_top - (char*)&stack_dummy;
}

#if MICROPY_STACK_CHECK

static uint stack_limit = 10240;

void mp_stack_set_limit(uint limit) {
    stack_limit = limit;
}

void mp_stack_check() {
    if (mp_stack_usage() >= stack_limit) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, "maximum recursion depth exceeded"));
    }
}

#endif // MICROPY_STACK_CHECK
