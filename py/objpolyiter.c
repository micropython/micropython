/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

#include <stdlib.h>

#include "py/nlr.h"
#include "py/runtime.h"

// This is universal iterator type which calls "iternext" method stored in
// particular object instance. (So, each instance of this time can have its
// own iteration behavior.) Having this type saves to define type objects
// for various internal iterator objects.

// Any instance should have these 2 fields at the beginning
typedef struct _mp_obj_polymorph_iter_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
} mp_obj_polymorph_iter_t;

STATIC mp_obj_t polymorph_it_iternext(mp_obj_t self_in) {
    mp_obj_polymorph_iter_t *self = MP_OBJ_TO_PTR(self_in);
    // Redirect call to object instance's iternext method
    return self->iternext(self_in);
}

const mp_obj_type_t mp_type_polymorph_iter = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity_getiter,
    .iternext = polymorph_it_iternext,
};
