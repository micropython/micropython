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
#include "py/builtin.h"

STATIC mp_obj_t op_getitem(mp_obj_t self_in, mp_obj_t key_in) {
    mp_obj_type_t *type = mp_obj_get_type(self_in);
    return type->subscr(self_in, key_in, MP_OBJ_SENTINEL);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_getitem_obj, op_getitem);

STATIC mp_obj_t op_setitem(mp_obj_t self_in, mp_obj_t key_in, mp_obj_t value_in) {
    mp_obj_type_t *type = mp_obj_get_type(self_in);
    return type->subscr(self_in, key_in, value_in);
}
MP_DEFINE_CONST_FUN_OBJ_3(mp_op_setitem_obj, op_setitem);

STATIC mp_obj_t op_delitem(mp_obj_t self_in, mp_obj_t key_in) {
    mp_obj_type_t *type = mp_obj_get_type(self_in);
    return type->subscr(self_in, key_in, MP_OBJ_NULL);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_delitem_obj, op_delitem);

STATIC mp_obj_t op_contains(mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_type_t *type = mp_obj_get_type(lhs_in);
    return type->binary_op(MP_BINARY_OP_CONTAINS, lhs_in, rhs_in);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_contains_obj, op_contains);
