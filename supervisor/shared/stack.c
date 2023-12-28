/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "stack.h"

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "supervisor/cpu.h"
#include "supervisor/port.h"
#include "supervisor/shared/safe_mode.h"

void stack_init(void) {
    uint32_t *stack_limit = port_stack_get_limit();
    *stack_limit = STACK_CANARY_VALUE;
}

inline bool stack_ok(void) {
    uint32_t *stack_limit = port_stack_get_limit();
    return *stack_limit == STACK_CANARY_VALUE;
}

inline void assert_heap_ok(void) {
    if (!stack_ok()) {
        reset_into_safe_mode(SAFE_MODE_STACK_OVERFLOW);
    }
}
