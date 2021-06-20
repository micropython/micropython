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

extern uint32_t _estack;

// Requested size.
static uint32_t next_stack_size = CIRCUITPY_DEFAULT_STACK_SIZE;
static uint32_t current_stack_size = 0;
// Actual location and size, may be larger than requested.
static uint32_t *stack_limit = NULL;
static size_t stack_length = 0;

#define EXCEPTION_STACK_SIZE 1024

void allocate_stack(void) {

    if (port_has_fixed_stack()) {
        stack_limit = port_stack_get_limit();
        stack_length = (port_stack_get_top() - stack_limit) * sizeof(uint32_t);
        current_stack_size = stack_length;
    } else {
        mp_uint_t regs[10];
        mp_uint_t sp = cpu_get_regs_and_sp(regs);

        mp_uint_t c_size = (uint32_t)port_stack_get_top() - sp;
        supervisor_allocation *stack_alloc = allocate_memory(c_size + next_stack_size + EXCEPTION_STACK_SIZE, true, false);
        if (stack_alloc == NULL) {
            stack_alloc = allocate_memory(c_size + CIRCUITPY_DEFAULT_STACK_SIZE + EXCEPTION_STACK_SIZE, true, false);
            current_stack_size = CIRCUITPY_DEFAULT_STACK_SIZE;
        } else {
            current_stack_size = next_stack_size;
        }
        stack_limit = stack_alloc->ptr;
        stack_length = get_allocation_length(stack_alloc);
    }

    *stack_limit = STACK_CANARY_VALUE;
}

inline bool stack_ok(void) {
    return stack_limit == NULL || *stack_limit == STACK_CANARY_VALUE;
}

inline void assert_heap_ok(void) {
    if (!stack_ok()) {
        reset_into_safe_mode(HEAP_OVERWRITTEN);
    }
}

void stack_init(void) {
    allocate_stack();
}

void stack_resize(void) {
    if (stack_limit == NULL) {
        return;
    }
    if (next_stack_size == current_stack_size) {
        *stack_limit = STACK_CANARY_VALUE;
        return;
    }
    free_memory(allocation_from_ptr(stack_limit));
    stack_limit = NULL;
    allocate_stack();
}

uint32_t *stack_get_bottom(void) {
    return stack_limit;
}

size_t stack_get_length(void) {
    return stack_length;
}

void set_next_stack_size(uint32_t size) {
    next_stack_size = size;
}

uint32_t get_current_stack_size(void) {
    return current_stack_size;
}
