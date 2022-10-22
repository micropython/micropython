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

#include "py/obj.h"

#if !MICROPY_OBJ_IMMEDIATE_OBJS
typedef struct _mp_obj_none_t {
    mp_obj_base_t base;
} mp_obj_none_t;
#endif

STATIC void none_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    if (MICROPY_PY_UJSON && kind == PRINT_JSON) {
        mp_print_str(print, "null");
    } else {
        mp_print_str(print, "None");
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_NoneType,
    MP_QSTR_NoneType,
    MP_TYPE_FLAG_NONE,
    print, none_print,
    unary_op, mp_generic_unary_op
    );

#if !MICROPY_OBJ_IMMEDIATE_OBJS
const mp_obj_none_t mp_const_none_obj = {{&mp_type_NoneType}};
#endif
