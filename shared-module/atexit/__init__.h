/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_ATEXIT___INIT___H
#define MICROPY_INCLUDED_SHARED_MODULE_ATEXIT___INIT___H

#include "py/obj.h"
#include "shared/runtime/pyexec.h"

typedef struct _atexit_callback_t {
    size_t n_pos, n_kw;
    mp_obj_t func, *args;
} atexit_callback_t;

extern void atexit_reset(void);
extern void atexit_gc_collect(void);

extern void shared_module_atexit_register(mp_obj_t *func,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern void shared_module_atexit_unregister(const mp_obj_t *func);
extern void shared_module_atexit_execute(pyexec_result_t *result);

#endif  // MICROPY_INCLUDED_SHARED_MODULE_ATEXIT___INIT___H
