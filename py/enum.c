/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "py/enum.h"
#include "py/runtime.h"

mp_obj_t cp_enum_find(const mp_obj_type_t *type, int value) {
    const mp_obj_dict_t *dict = type->locals_dict;
    for (size_t i = 0; i < dict->map.used; i++) {
        const cp_enum_obj_t *v = dict->map.table[i].value;
        if (v->value == value) {
            return (mp_obj_t)v;
        }
    }
    return mp_const_none;
}

int cp_enum_value(const mp_obj_type_t *type, mp_obj_t *obj) {
    if (!mp_obj_is_type(obj, type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("Expected a %q"), type->name);
    }
    return ((cp_enum_obj_t *)MP_OBJ_TO_PTR(obj))->value;
}

void cp_enum_obj_print_helper(uint16_t module, const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    cp_enum_obj_t *self = self_in;
    mp_printf(print, "%q.%q.%q", module, self->base.type->name, self->name);
}
