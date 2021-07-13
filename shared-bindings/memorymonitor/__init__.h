/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR___INIT___H

#include "py/obj.h"


void memorymonitor_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);

#define MP_DEFINE_MEMORYMONITOR_EXCEPTION(exc_name, base_name) \
    const mp_obj_type_t mp_type_memorymonitor_##exc_name = { \
        { &mp_type_type }, \
        .name = MP_QSTR_##exc_name, \
        .print = memorymonitor_exception_print, \
        .make_new = mp_obj_exception_make_new, \
        .attr = mp_obj_exception_attr, \
        .parent = &mp_type_##base_name, \
    };

extern const mp_obj_type_t mp_type_memorymonitor_AllocationError;

NORETURN void mp_raise_memorymonitor_AllocationError(const compressed_string_t *msg, ...);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR___INIT___H
