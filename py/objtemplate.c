/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Koudai Aono
 * Copyright (c) 2026 Damien P. George
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

#if MICROPY_PY_TSTRINGS

typedef struct _mp_obj_template_t {
    mp_obj_base_t base;
    mp_obj_t strings;
    mp_obj_t interpolations;
} mp_obj_template_t;

typedef struct _mp_obj_interpolation_t {
    mp_obj_base_t base;
    mp_obj_t value;
    mp_obj_t expression;
    mp_obj_t conversion;
    mp_obj_t format_spec;
} mp_obj_interpolation_t;

mp_obj_t mp_obj_new_interpolation(mp_obj_t value, mp_obj_t expr, mp_obj_t conv, mp_obj_t spec);

static mp_obj_t mp_obj_template_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, false);

    mp_obj_t strings_obj;
    mp_obj_t interpolations_obj;

    if (n_args == 0) {
        mp_obj_t empty = MP_OBJ_NEW_QSTR(MP_QSTR_);
        strings_obj = mp_obj_new_tuple(1, &empty);
        interpolations_obj = mp_obj_new_tuple(0, NULL);
    } else {
        size_t n_interpolations = 0;
        size_t n_str_args = 0;
        for (size_t i = 0; i < n_args; i++) {
            if (mp_obj_is_exact_type(args[i], &mp_type_interpolation)) {
                n_interpolations++;
            } else if (mp_obj_is_str(args[i])) {
                n_str_args++;
            } else {
                mp_raise_TypeError(MP_ERROR_TEXT("expected str or Interpolation"));
            }
        }

        if (n_interpolations == 0) {
            if (n_str_args == 1) {
                strings_obj = mp_obj_new_tuple(1, &args[0]);
            } else {
                size_t total_len = 0;
                for (size_t i = 0; i < n_args; i++) {
                    size_t str_len;
                    (void)mp_obj_str_get_data(args[i], &str_len);
                    total_len += str_len;
                }
                vstr_t vstr;
                vstr_init(&vstr, total_len);
                for (size_t i = 0; i < n_args; i++) {
                    size_t str_len;
                    const char *str_data = mp_obj_str_get_data(args[i], &str_len);
                    vstr_add_strn(&vstr, str_data, str_len);
                }
                mp_obj_t str_items[1];
                str_items[0] = mp_obj_new_str_from_vstr(&vstr);
                strings_obj = mp_obj_new_tuple(1, str_items);
            }
            interpolations_obj = mp_obj_new_tuple(0, NULL);
        } else {
            size_t n_strings = n_interpolations + 1;
            mp_obj_tuple_t *strings_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, n_strings, &mp_type_tuple);
            mp_obj_tuple_t *interpolations_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, n_interpolations, &mp_type_tuple);
            strings_tuple->len = n_strings;
            interpolations_tuple->len = n_interpolations;

            size_t string_idx = 0;
            size_t interp_idx = 0;
            mp_obj_t current_str = MP_OBJ_NULL;
            bool current_vstr_active = false;
            vstr_t current_vstr = {0};

            for (size_t i = 0; i <= n_args; i++) {
                if (i == n_args || mp_obj_is_exact_type(args[i], &mp_type_interpolation)) {
                    mp_obj_t out_str;
                    if (current_vstr_active) {
                        out_str = mp_obj_new_str_from_vstr(&current_vstr);
                        current_vstr_active = false;
                    } else if (current_str != MP_OBJ_NULL) {
                        out_str = current_str;
                    } else {
                        out_str = MP_OBJ_NEW_QSTR(MP_QSTR_);
                    }
                    strings_tuple->items[string_idx++] = out_str;
                    current_str = MP_OBJ_NULL;
                    if (i < n_args) {
                        interpolations_tuple->items[interp_idx++] = args[i];
                    }
                } else {
                    size_t str_len;
                    const char *str_data = mp_obj_str_get_data(args[i], &str_len);
                    if (current_vstr_active) {
                        vstr_add_strn(&current_vstr, str_data, str_len);
                    } else if (current_str == MP_OBJ_NULL) {
                        current_str = args[i];
                    } else {
                        size_t prev_len;
                        const char *prev_data = mp_obj_str_get_data(current_str, &prev_len);
                        vstr_init(&current_vstr, prev_len + str_len);
                        vstr_add_strn(&current_vstr, prev_data, prev_len);
                        vstr_add_strn(&current_vstr, str_data, str_len);
                        current_vstr_active = true;
                        current_str = MP_OBJ_NULL;
                    }
                }
            }

            strings_obj = MP_OBJ_FROM_PTR(strings_tuple);
            interpolations_obj = MP_OBJ_FROM_PTR(interpolations_tuple);
        }
    }

    mp_obj_template_t *self = mp_obj_malloc(mp_obj_template_t, type);
    self->strings = strings_obj;
    self->interpolations = interpolations_obj;
    return MP_OBJ_FROM_PTR(self);
}

static void mp_obj_template_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q(%q=", MP_QSTR_Template, MP_QSTR_strings);
    mp_obj_print_helper(print, self->strings, PRINT_REPR);
    mp_printf(print, ", %q=", MP_QSTR_interpolations);
    mp_obj_print_helper(print, self->interpolations, PRINT_REPR);
    mp_print_str(print, ")");
}

static mp_obj_t mp_obj_template_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_template_t *lhs = MP_OBJ_TO_PTR(lhs_in);

    switch (op) {
        case MP_BINARY_OP_ADD: {
            if (!mp_obj_is_exact_type(rhs_in, &mp_type_template)) {
                return MP_OBJ_NULL; // op not supported
            }

            mp_obj_template_t *rhs = MP_OBJ_TO_PTR(rhs_in);

            mp_obj_tuple_t *lhs_strings = MP_OBJ_TO_PTR(lhs->strings);
            mp_obj_tuple_t *lhs_interps = MP_OBJ_TO_PTR(lhs->interpolations);
            mp_obj_tuple_t *rhs_strings = MP_OBJ_TO_PTR(rhs->strings);
            mp_obj_tuple_t *rhs_interps = MP_OBJ_TO_PTR(rhs->interpolations);

            size_t new_strings_len = lhs_strings->len + rhs_strings->len - 1;
            size_t new_interps_len = lhs_interps->len + rhs_interps->len;

            // Create tuples directly to avoid GC issues
            mp_obj_tuple_t *new_strings_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, new_strings_len, &mp_type_tuple);
            mp_obj_tuple_t *new_interps_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, new_interps_len, &mp_type_tuple);
            new_strings_tuple->len = new_strings_len;
            new_interps_tuple->len = new_interps_len;

            // Copy all but the last string from lhs
            for (size_t i = 0; i < lhs_strings->len - 1; i++) {
                new_strings_tuple->items[i] = lhs_strings->items[i];
            }

            // Merge last string from lhs with first string from rhs
            size_t lhs_last_len, rhs_first_len;
            const char *lhs_last_str = mp_obj_str_get_data(lhs_strings->items[lhs_strings->len - 1], &lhs_last_len);
            const char *rhs_first_str = mp_obj_str_get_data(rhs_strings->items[0], &rhs_first_len);

            vstr_t vstr;
            vstr_init(&vstr, lhs_last_len + rhs_first_len);
            vstr_add_strn(&vstr, lhs_last_str, lhs_last_len);
            vstr_add_strn(&vstr, rhs_first_str, rhs_first_len);
            new_strings_tuple->items[lhs_strings->len - 1] = mp_obj_new_str_from_vstr(&vstr);

            // Copy remaining strings from rhs
            for (size_t i = 1; i < rhs_strings->len; i++) {
                new_strings_tuple->items[lhs_strings->len - 1 + i] = rhs_strings->items[i];
            }

            // Copy interpolations from both sides
            for (size_t i = 0; i < lhs_interps->len; i++) {
                new_interps_tuple->items[i] = lhs_interps->items[i];
            }

            for (size_t i = 0; i < rhs_interps->len; i++) {
                new_interps_tuple->items[lhs_interps->len + i] = rhs_interps->items[i];
            }

            mp_obj_template_t *result = mp_obj_malloc(mp_obj_template_t, &mp_type_template);
            result->strings = MP_OBJ_FROM_PTR(new_strings_tuple);
            result->interpolations = MP_OBJ_FROM_PTR(new_interps_tuple);
            return MP_OBJ_FROM_PTR(result);
        }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t template_str(mp_obj_t self_in) {
    // Same as __repr__(), following CPython behavior
    mp_print_t print;
    vstr_t vstr;
    vstr_init_print(&vstr, 16, &print);
    mp_obj_template_print(&print, self_in, PRINT_STR);
    return mp_obj_new_str_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(template_str_obj, template_str);

static void mp_obj_template_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        if (attr == MP_QSTR_strings) {
            dest[0] = self->strings;
        } else if (attr == MP_QSTR_interpolations) {
            dest[0] = self->interpolations;
        } else if (attr == MP_QSTR_values) {
            mp_obj_tuple_t *interps = MP_OBJ_TO_PTR(self->interpolations);
            mp_obj_tuple_t *values_tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(interps->len, NULL));
            for (size_t i = 0; i < interps->len; i++) {
                mp_obj_interpolation_t *interp = MP_OBJ_TO_PTR(interps->items[i]);
                values_tuple->items[i] = interp->value;
            }
            dest[0] = MP_OBJ_FROM_PTR(values_tuple);
        } else if (attr == MP_QSTR___str__) {
            dest[0] = MP_OBJ_FROM_PTR(&template_str_obj);
            dest[1] = self_in;
        }
    }
}

typedef struct _mp_obj_template_iter_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t template;
    size_t index;
} mp_obj_template_iter_t;

static mp_obj_t template_iter_iternext(mp_obj_t self_in) {
    mp_obj_template_iter_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_template_t *tmpl = MP_OBJ_TO_PTR(self->template);
    mp_obj_tuple_t *strings = MP_OBJ_TO_PTR(tmpl->strings);
    mp_obj_tuple_t *interps = MP_OBJ_TO_PTR(tmpl->interpolations);

    while (self->index < strings->len + interps->len) {
        if ((self->index & 1) == 0) {
            // string
            mp_obj_t str_obj = strings->items[self->index++ / 2];
            size_t str_len;
            mp_obj_str_get_data(str_obj, &str_len);
            if (str_len > 0) {
                return str_obj;
            }
        } else {
            // interpolation
            return interps->items[self->index++ / 2];
        }
    }

    return MP_OBJ_STOP_ITERATION;
}

static mp_obj_t mp_obj_template_iter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_template_iter_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_template_iter_t *iter = (mp_obj_template_iter_t *)iter_buf;
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = template_iter_iternext;
    iter->template = self_in;
    iter->index = 0;
    return MP_OBJ_FROM_PTR(iter);
}

mp_obj_t mp_obj_new_template(size_t n_args, const mp_obj_t *args) {
    #if 0
    printf("__template__(");
    for (size_t i = 0; i < n_args; ++i) {
        mp_obj_print(args[i], PRINT_REPR);
        printf(", ");
    }
    printf(")\n");
    #endif

    mp_obj_template_t *o = mp_obj_malloc(mp_obj_template_t, &mp_type_template);
    o->strings = args[0];
    size_t n_interpolations = (n_args - 1) / 4;
    mp_obj_tuple_t *interpolations = MP_OBJ_TO_PTR(mp_obj_new_tuple(n_interpolations, NULL));
    for (size_t i = 0; i < n_interpolations; ++i) {
        interpolations->items[i] = mp_obj_new_interpolation(args[i * 4 + 1], args[i * 4 + 2], args[i * 4 + 3], args[i * 4 + 4]);
    }
    o->interpolations = MP_OBJ_FROM_PTR(interpolations);
    return MP_OBJ_FROM_PTR(o);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_template,
    MP_QSTR_Template,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_template_make_new,
    print, mp_obj_template_print,
    binary_op, mp_obj_template_binary_op,
    attr, mp_obj_template_attr,
    iter, mp_obj_template_iter
    );

/////////////////////////////////////////////////////////////////

static mp_obj_t mp_obj_interpolation_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
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

static void mp_obj_interpolation_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_interpolation_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q(", MP_QSTR_Interpolation);
    mp_obj_print_helper(print, self->value, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->expression, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->conversion, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->format_spec, PRINT_REPR);
    mp_print_str(print, ")");
}

static void mp_obj_interpolation_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_interpolation_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        if (attr == MP_QSTR_value) {
            dest[0] = self->value;
        } else if (attr == MP_QSTR_expression) {
            dest[0] = self->expression;
        } else if (attr == MP_QSTR_conversion) {
            dest[0] = self->conversion;
        } else if (attr == MP_QSTR_format_spec) {
            dest[0] = self->format_spec;
        }
    }
}

mp_obj_t mp_obj_new_interpolation(mp_obj_t value, mp_obj_t expression, mp_obj_t conversion, mp_obj_t format_spec) {
    mp_obj_interpolation_t *o = mp_obj_malloc(mp_obj_interpolation_t, &mp_type_interpolation);
    o->value = value;
    o->expression = expression;
    o->conversion = conversion;
    o->format_spec = format_spec;
    return MP_OBJ_FROM_PTR(o);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_interpolation,
    MP_QSTR_Interpolation,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_interpolation_make_new,
    print, mp_obj_interpolation_print,
    attr, mp_obj_interpolation_attr
    );

#endif // MICROPY_PY_TSTRINGS
