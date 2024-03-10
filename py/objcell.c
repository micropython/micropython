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

#include "py/obj.h"

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
STATIC void cell_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_cell_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "<cell %p ", o->obj);
    if (o->obj == MP_OBJ_NULL) {
        mp_print_str(print, "(nil)");
    } else {
        mp_obj_print_helper(print, o->obj, PRINT_REPR);
    }
    mp_print_str(print, ">");
}
#endif

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
#define CELL_TYPE_PRINT , print, cell_print
#else
#define CELL_TYPE_PRINT
#endif

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    // cell representation is just value in < >
    mp_type_cell, MP_QSTR_, MP_TYPE_FLAG_NONE
    CELL_TYPE_PRINT
    );

mp_obj_t mp_obj_new_cell(mp_obj_t obj) {
    mp_obj_cell_t *o = mp_obj_malloc(mp_obj_cell_t, &mp_type_cell);
    o->obj = obj;
    return MP_OBJ_FROM_PTR(o);
}
