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

#include <stdlib.h>
#include <assert.h>

#include "py/obj.h"

/******************************************************************************/
/* singleton objects defined by Python                                        */

typedef struct _mp_obj_singleton_t {
    mp_obj_base_t base;
    qstr name;
} mp_obj_singleton_t;

STATIC void singleton_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_singleton_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q", self->name);
}

const mp_obj_type_t mp_type_singleton = {
    { &mp_type_type },
    .name = MP_QSTR_,
    .print = singleton_print,
};

const mp_obj_singleton_t mp_const_ellipsis_obj = {{&mp_type_singleton}, MP_QSTR_Ellipsis};
#if MICROPY_PY_BUILTINS_NOTIMPLEMENTED
const mp_obj_singleton_t mp_const_notimplemented_obj = {{&mp_type_singleton}, MP_QSTR_NotImplemented};
#endif
