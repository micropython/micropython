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

#include "py/runtime.h"
#include "py/obj.h"

#if MICROPY_PY_TSTRINGS

// Interpolation type for PEP 750 template strings
typedef struct _mp_obj_interpolation_t {
    mp_obj_base_t base;
    mp_obj_t value;
    mp_obj_t expression;
    mp_obj_t conversion;    // None or single char string ('r', 's', 'a')
    mp_obj_t format_spec;   // string
} mp_obj_interpolation_t;

// Forward declarations
const mp_obj_type_t mp_type_interpolation;

// Constructor
static mp_obj_t interpolation_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 4, false);

    mp_obj_interpolation_t *self = mp_obj_malloc(mp_obj_interpolation_t, &mp_type_interpolation);
    self->value = args[0];
    self->expression = args[1];
    self->conversion = (n_args > 2) ? args[2] : mp_const_none;
    self->format_spec = (n_args > 3) ? args[3] : MP_OBJ_NEW_QSTR(MP_QSTR_);

    return MP_OBJ_FROM_PTR(self);
}

// Attribute access (read-only for immutability)
static void interpolation_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_interpolation_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute
        if (attr == MP_QSTR_value) {
            dest[0] = self->value;
        } else if (attr == MP_QSTR_expression) {
            dest[0] = self->expression;
        } else if (attr == MP_QSTR_conversion) {
            dest[0] = self->conversion;
        } else if (attr == MP_QSTR_format_spec) {
            dest[0] = self->format_spec;
        } else if (attr == MP_QSTR___match_args__) {
            // For pattern matching support - create tuple dynamically
            mp_obj_t match_args_items[4] = {
                MP_OBJ_NEW_QSTR(MP_QSTR_value),
                MP_OBJ_NEW_QSTR(MP_QSTR_expression),
                MP_OBJ_NEW_QSTR(MP_QSTR_conversion),
                MP_OBJ_NEW_QSTR(MP_QSTR_format_spec),
            };
            dest[0] = mp_obj_new_tuple(4, match_args_items);
        }
    } else {
        // Store attribute - not allowed (immutable per PEP 750)
        if (dest[1] != MP_OBJ_NULL) {
            mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("'Interpolation' object attribute is read-only"));
        }
    }
}

// repr() implementation
static void interpolation_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;  // Only repr is supported
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

// PEP 750: Template and Interpolation instances compare with object identity (is)
// No custom equality comparison is provided

// Type definition
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_interpolation,
    MP_QSTR_Interpolation,
    MP_TYPE_FLAG_NONE,
    make_new, interpolation_make_new,
    print, interpolation_print,
    attr, interpolation_attr
    );

// Helper function for C code to create Interpolation objects
mp_obj_t mp_obj_new_interpolation(mp_obj_t value, mp_obj_t expression, mp_obj_t conversion, mp_obj_t format_spec) {
    mp_obj_t args[4] = {value, expression, conversion, format_spec};
    return interpolation_make_new(&mp_type_interpolation, 4, 0, args);
}

#else // MICROPY_PY_TSTRINGS

// Stub implementations when t-strings are disabled
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_interpolation,
    MP_QSTR_Interpolation,
    MP_TYPE_FLAG_NONE
    );

mp_obj_t mp_obj_new_interpolation(mp_obj_t value, mp_obj_t expression, mp_obj_t conversion, mp_obj_t format_spec) {
    (void)value;
    (void)expression;
    (void)conversion;
    (void)format_spec;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("template strings not available"));
    return mp_const_none;  // Never reached, but needed for compiler
}

#endif // MICROPY_PY_TSTRINGS
