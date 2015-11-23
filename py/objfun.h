/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef __MICROPY_INCLUDED_PY_OBJFUN_H__
#define __MICROPY_INCLUDED_PY_OBJFUN_H__

#include "py/obj.h"

typedef struct _mp_obj_fun_bc_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;         // the context within which this function was defined
    const byte *bytecode;           // bytecode for the function
    const mp_uint_t *const_table;   // constant table
    // the following extra_args array is allocated space to take (in order):
    //  - values of positional default args (if any)
    //  - a single slot for default kw args dict (if it has them)
    //  - a single slot for var args tuple (if it takes them)
    //  - a single slot for kw args dict (if it takes them)
    mp_obj_t extra_args[];
} mp_obj_fun_bc_t;

#endif // __MICROPY_INCLUDED_PY_OBJFUN_H__
