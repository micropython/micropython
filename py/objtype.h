/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_OBJTYPE_H
#define MICROPY_INCLUDED_PY_OBJTYPE_H

#include "py/obj.h"

// instance object
// creating an instance of a class makes one of these objects
typedef struct _mp_obj_instance_t {
    mp_obj_base_t base;
    mp_map_t members;
    mp_obj_t subobj[];
    // TODO maybe cache __getattr__ and __setattr__ for efficient lookup of them
} mp_obj_instance_t;

// this needs to be exposed for MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE to work
void mp_obj_instance_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);

// these need to be exposed so mp_obj_is_callable can work correctly
bool mp_obj_instance_is_callable(mp_obj_t self_in);
mp_obj_t mp_obj_instance_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args);

#define mp_obj_is_instance_type(type) ((type)->make_new == mp_obj_instance_make_new)
#define mp_obj_is_native_type(type) ((type)->make_new != mp_obj_instance_make_new)
// this needs to be exposed for the above macros to work correctly
mp_obj_t mp_obj_instance_make_new(const mp_obj_type_t *self_in, size_t n_args, size_t n_kw, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_PY_OBJTYPE_H
