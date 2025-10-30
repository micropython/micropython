/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Koudai Aono
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

#include "py/mpconfig.h"

#if MICROPY_PY_TSTRINGS

#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtstring.h"

const mp_obj_type_t mp_type_interpolation;

static mp_obj_t interpolation_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_value, ARG_expression, ARG_conversion, ARG_format_spec };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_expression, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_)} },
        { MP_QSTR_conversion, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_format_spec, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_interpolation_t *self = mp_obj_malloc(mp_obj_interpolation_t, &mp_type_interpolation);
    self->value = args[ARG_value].u_obj;
    self->expression = args[ARG_expression].u_obj;
    self->conversion = args[ARG_conversion].u_obj;
    self->format_spec = args[ARG_format_spec].u_obj;

    return MP_OBJ_FROM_PTR(self);
}

static void interpolation_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_interpolation_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        if (attr == MP_QSTR_value) {
            dest[0] = self->value;
        } else if (attr == MP_QSTR_expression) {
            dest[0] = self->expression;
        } else if (attr == MP_QSTR_conversion) {
            dest[0] = self->conversion;
        } else if (attr == MP_QSTR_format_spec) {
            dest[0] = self->format_spec;
        }
    } else {
        if (dest[1] != MP_OBJ_NULL) {
            mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("readonly attribute"));
        }
    }
}

static void interpolation_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_interpolation_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "Interpolation(");
    mp_obj_print_helper(print, self->value, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->expression, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->conversion, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->format_spec, PRINT_REPR);
    mp_print_str(print, ")");
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_interpolation,
    MP_QSTR_Interpolation,
    MP_TYPE_FLAG_NONE,
    make_new, interpolation_make_new,
    print, interpolation_print,
    attr, interpolation_attr
    );

mp_obj_t mp_obj_new_interpolation(mp_obj_t value, mp_obj_t expression, mp_obj_t conversion, mp_obj_t format_spec) {
    mp_obj_t args[4] = {value, expression, conversion, format_spec};
    return interpolation_make_new(&mp_type_interpolation, 4, 0, args);
}

#endif
