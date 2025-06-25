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

#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objexcept.h"

#if MICROPY_PY_TSTRINGS

// Forward declarations
extern const mp_obj_type_t mp_type_template;
extern const mp_obj_type_t mp_type_interpolation;

// Template string (t-string) support as specified in PEP 750
typedef struct _mp_obj_template_t {
    mp_obj_base_t base;
    mp_obj_t strings;
    mp_obj_t interpolations;
} mp_obj_template_t;

static void template_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "Template(strings=");
    mp_obj_print_helper(print, self->strings, PRINT_REPR);
    mp_print_str(print, ", interpolations=");
    mp_obj_print_helper(print, self->interpolations, PRINT_REPR);
    mp_print_str(print, ")");
}

static mp_obj_t template_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    // Validate that strings is a tuple
    if (!mp_obj_is_type(args[0], &mp_type_tuple)) {
        mp_raise_TypeError(MP_ERROR_TEXT("strings must be a tuple"));
    }

    // Validate that all elements in strings are strings
    mp_obj_tuple_t *strings_tuple = MP_OBJ_TO_PTR(args[0]);
    for (size_t i = 0; i < strings_tuple->len; i++) {
        if (!mp_obj_is_str(strings_tuple->items[i])) {
            mp_raise_TypeError(MP_ERROR_TEXT("all strings elements must be strings"));
        }
    }

    // Validate that interpolations is a tuple
    if (!mp_obj_is_type(args[1], &mp_type_tuple)) {
        mp_raise_TypeError(MP_ERROR_TEXT("interpolations must be a tuple"));
    }

    // Validate that all elements in interpolations are Interpolation objects
    mp_obj_tuple_t *interps_tuple = MP_OBJ_TO_PTR(args[1]);
    for (size_t i = 0; i < interps_tuple->len; i++) {
        if (!mp_obj_is_type(interps_tuple->items[i], &mp_type_interpolation)) {
            mp_raise_TypeError(MP_ERROR_TEXT("all interpolations elements must be Interpolation objects"));
        }
    }

    mp_obj_template_t *self = m_new_obj(mp_obj_template_t);
    self->base.type = type;
    self->strings = args[0];
    self->interpolations = args[1];
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t template_str(mp_obj_t self_in) {
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);

    // Convert template to string by interleaving strings and interpolation values
    mp_obj_tuple_t *strings = MP_OBJ_TO_PTR(self->strings);
    mp_obj_tuple_t *interps = MP_OBJ_TO_PTR(self->interpolations);

    vstr_t vstr;
    vstr_init(&vstr, 16);
    size_t total_size = 0;

    for (size_t i = 0; i < strings->len; i++) {
        mp_obj_t str_part = strings->items[i];
        size_t str_len;
        const char *str_data = mp_obj_str_get_data(str_part, &str_len);

        total_size += str_len;
        if (total_size > MICROPY_PY_TSTRING_MAX_TEMPLATE_SIZE) {
            mp_raise_ValueError(MP_ERROR_TEXT("template string too large"));
        }

        vstr_add_strn(&vstr, str_data, str_len);

        if (i < interps->len) {
            mp_obj_t interp_obj = interps->items[i];

            mp_obj_t value = mp_load_attr(interp_obj, MP_QSTR_value);
            mp_obj_t expression = mp_load_attr(interp_obj, MP_QSTR_expression);
            mp_obj_t conversion = mp_load_attr(interp_obj, MP_QSTR_conversion);
            mp_obj_t format_spec = mp_load_attr(interp_obj, MP_QSTR_format_spec);

            // Handle debug format (expression ends with '=')
            size_t expr_len;
            const char *expr_str = mp_obj_str_get_data(expression, &expr_len);
            if (expr_len > 0 && expr_str[expr_len - 1] == '=') {
                // Debug format: prepend expression without '='
                vstr_add_strn(&vstr, expr_str, expr_len - 1);
                vstr_add_byte(&vstr, '=');
            }

            const char *conversion_str = NULL;
            const char *format_spec_str = "";
            if (conversion != mp_const_none) {
                conversion_str = mp_obj_str_get_str(conversion);
            }
            format_spec_str = mp_obj_str_get_str(format_spec);

            mp_obj_t conv_value = value;
            if (conversion_str && *conversion_str) {
                if (*conversion_str == 'r') {
                    vstr_t repr_vstr;
                    vstr_init(&repr_vstr, 16);
                    mp_print_t repr_print;
                    vstr_init_print(&repr_vstr, 16, &repr_print);
                    mp_obj_print_helper(&repr_print, value, PRINT_REPR);
                    conv_value = mp_obj_new_str_from_vstr(&repr_vstr);
                } else if (*conversion_str == 's') {
                    vstr_t str_vstr;
                    vstr_init(&str_vstr, 16);
                    mp_print_t str_print;
                    vstr_init_print(&str_vstr, 16, &str_print);
                    mp_obj_print_helper(&str_print, value, PRINT_STR);
                    conv_value = mp_obj_new_str_from_vstr(&str_vstr);
                } else if (*conversion_str == 'a') {
                    vstr_t repr_vstr;
                    vstr_init(&repr_vstr, 16);
                    mp_print_t repr_print;
                    vstr_init_print(&repr_vstr, 16, &repr_print);
                    mp_obj_print_helper(&repr_print, value, PRINT_REPR);
                    conv_value = mp_obj_new_str_from_vstr(&repr_vstr);
                }
            }

            if (*format_spec_str) {
                vstr_t fmt_vstr;
                vstr_init(&fmt_vstr, 16);
                vstr_add_str(&fmt_vstr, "{:");
                vstr_add_str(&fmt_vstr, format_spec_str);
                vstr_add_str(&fmt_vstr, "}");
                mp_obj_t fmt_str = mp_obj_new_str_from_vstr(&fmt_vstr);
                mp_obj_t fmt_args[2] = {fmt_str, conv_value};
                conv_value = mp_obj_str_format(2, fmt_args, NULL);
            } else {
                if (conversion_str == NULL) {
                    vstr_t str_vstr;
                    vstr_init(&str_vstr, 16);
                    mp_print_t str_print;
                    vstr_init_print(&str_vstr, 16, &str_print);
                    mp_obj_print_helper(&str_print, value, PRINT_STR);
                    conv_value = mp_obj_new_str_from_vstr(&str_vstr);
                }
            }

            size_t val_len;
            const char *val_str = mp_obj_str_get_data(conv_value, &val_len);
            vstr_add_strn(&vstr, val_str, val_len);
        }
    }

    return mp_obj_new_str_from_vstr(&vstr);
}

MP_DEFINE_CONST_FUN_OBJ_1(template_str_obj, template_str);

// Template iterator implementation
typedef struct _mp_obj_template_iter_t {
    mp_obj_base_t base;
    mp_obj_t template;
    size_t index;
    bool at_interpolation;
} mp_obj_template_iter_t;

static mp_obj_t template_iter_iternext(mp_obj_t self_in) {
    mp_obj_template_iter_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_template_t *tmpl = MP_OBJ_TO_PTR(self->template);
    mp_obj_tuple_t *strings = MP_OBJ_TO_PTR(tmpl->strings);
    mp_obj_tuple_t *interps = MP_OBJ_TO_PTR(tmpl->interpolations);

    // Alternate between strings and interpolations, skipping empty strings
    while (self->index < strings->len || (self->index < interps->len && self->at_interpolation)) {
        if (!self->at_interpolation) {
            // Try to return a string part
            if (self->index < strings->len) {
                mp_obj_t str_obj = strings->items[self->index];
                size_t str_len;
                mp_obj_str_get_data(str_obj, &str_len);

                // Skip empty strings as per PEP 750
                if (str_len > 0) {
                    self->at_interpolation = true;  // Next time check interpolation
                    return str_obj;
                } else {
                    // Empty string, skip to interpolation
                    self->at_interpolation = true;
                }
            }
        }

        if (self->at_interpolation) {
            // Try to return an interpolation
            if (self->index < interps->len) {
                mp_obj_t interp = interps->items[self->index];
                self->index++;  // Move to next position
                self->at_interpolation = false;  // Next time check string
                return interp;  // Return the Interpolation object directly
            } else {
                // No more interpolations, move to next string
                self->index++;
                self->at_interpolation = false;
            }
        }
    }

    return MP_OBJ_STOP_ITERATION;
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_template_iter,
    MP_QSTR_template_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, template_iter_iternext
    );

static mp_obj_t template_iter(mp_obj_t self_in) {
    mp_obj_template_iter_t *iter = mp_obj_malloc(mp_obj_template_iter_t, &mp_type_template_iter);
    iter->template = self_in;
    iter->index = 0;
    iter->at_interpolation = false;
    return MP_OBJ_FROM_PTR(iter);
}
MP_DEFINE_CONST_FUN_OBJ_1(template_iter_obj, template_iter);

static void template_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        if (attr == MP_QSTR_strings) {
            dest[0] = self->strings;
        } else if (attr == MP_QSTR_interpolations) {
            dest[0] = self->interpolations;
        } else if (attr == MP_QSTR_values) {
            mp_obj_tuple_t *interps = MP_OBJ_TO_PTR(self->interpolations);

            // Use stack allocation for small cases
            if (interps->len <= 4) {
                mp_obj_t values[4];
                for (size_t i = 0; i < interps->len; i++) {
                    values[i] = mp_load_attr(interps->items[i], MP_QSTR_value);
                }
                dest[0] = mp_obj_new_tuple(interps->len, values);
            } else {
                // Heap allocation with proper cleanup for larger cases
                mp_obj_t *values = m_new(mp_obj_t, interps->len);
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    for (size_t i = 0; i < interps->len; i++) {
                        values[i] = mp_load_attr(interps->items[i], MP_QSTR_value);
                    }
                    mp_obj_t result = mp_obj_new_tuple(interps->len, values);
                    nlr_pop();
                    m_del(mp_obj_t, values, interps->len);
                    dest[0] = result;
                } else {
                    // Clean up on exception
                    m_del(mp_obj_t, values, interps->len);
                    nlr_jump(nlr.ret_val);
                }
            }
        } else if (attr == MP_QSTR___str__) {
            // Provide __str__ method as a bound method
            dest[0] = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&template_str_obj), self_in);
        } else if (attr == MP_QSTR___iter__) {
            // Provide __iter__ method as a bound method
            dest[0] = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&template_iter_obj), self_in);
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_template,
    MP_QSTR_Template,
    MP_TYPE_FLAG_NONE,
    make_new, template_make_new,
    print, template_print,
    attr, template_attr,
    iter, template_iter
    );

// __template__ builtin function to create Template objects
static mp_obj_t mp_builtin___template__(mp_obj_t strings, mp_obj_t interpolations_in) {
    // Convert interpolation tuples to Interpolation objects
    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(interpolations_in, &len, &items);

    mp_obj_t *interpolations = m_new(mp_obj_t, len);
    for (size_t i = 0; i < len; i++) {
        // Each interpolation is a tuple (value, expression, conversion, format_spec)
        mp_obj_t *interp_items;
        size_t interp_len;
        mp_obj_get_array(items[i], &interp_len, &interp_items);

        if (interp_len >= 2) {
            mp_obj_t value = interp_items[0];
            mp_obj_t expression = interp_items[1];
            mp_obj_t conversion = (interp_len > 2) ? interp_items[2] : mp_const_none;
            mp_obj_t format_spec = (interp_len > 3) ? interp_items[3] : MP_OBJ_NEW_QSTR(MP_QSTR_);

            interpolations[i] = mp_obj_new_interpolation(value, expression, conversion, format_spec);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid interpolation format"));
        }
    }

    mp_obj_t interpolations_tuple = mp_obj_new_tuple(len, interpolations);
    m_del(mp_obj_t, interpolations, len);

    return template_make_new(&mp_type_template, 2, 0, (mp_obj_t[]) {strings, interpolations_tuple});
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin___template___obj, mp_builtin___template__);

#endif // MICROPY_PY_TSTRINGS