/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert
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

#include "py/mpstate.h"
#include "py/stackctrl.h"

#include "shared-bindings/ustack/__init__.h"

#if MICROPY_MAX_STACK_USAGE
uint32_t shared_module_ustack_max_stack_usage(void) {
    // Start at stack limit and move up.
    // Untouched stack was filled with a sentinel value.
    // Stop at first non-sentinel byte.
    char *p = MP_STATE_THREAD(stack_bottom);
    while (*p++ == MP_MAX_STACK_USAGE_SENTINEL_BYTE) {
    }
    return MP_STATE_THREAD(stack_top) - p;
}
#endif

uint32_t shared_module_ustack_stack_size() {
    return MP_STATE_THREAD(stack_limit);
}

uint32_t shared_module_ustack_stack_usage() {
    return mp_stack_usage();
}
