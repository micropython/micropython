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
#include "py/misc.h"
#include "py/objtstring.h"

#if MICROPY_PY_TSTRINGS

#define TEMPLATE_COUNT_MAX (0x0FFF)

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
    if (n_kw != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("Template.__new__ only accepts *args arguments"));
    }

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
                // Shortened message for all builds to avoid stack overflow during ROM compression on constrained platforms (Windows x86, ASan)
                mp_raise_msg_varg(&mp_type_TypeError,
                    MP_ERROR_TEXT("Template.__new__ args must be str or Interpolation, got '%s'"),
                    mp_obj_get_type_str(args[i]));
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
            if (n_interpolations > TEMPLATE_COUNT_MAX) {
                mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("template string too large for header format"));
            }

            size_t n_strings = n_interpolations + 1;
            if (n_strings > TEMPLATE_COUNT_MAX) {
                mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("template string too large for header format"));
            }
            mp_obj_tuple_t *strings_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, n_strings, &mp_type_tuple);
            mp_obj_tuple_t *interpolations_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, n_interpolations, &mp_type_tuple);
            strings_tuple->len = n_strings;
            interpolations_tuple->len = n_interpolations;

            size_t string_idx = 0;
            size_t interp_idx = 0;
            mp_obj_t current_str = MP_OBJ_NULL;
            bool current_vstr_active = false;
            vstr_t current_vstr = {0};

            #define FLUSH_CURRENT_STRING() \
    do { \
        mp_obj_t out_str; \
        if (current_vstr_active) { \
            out_str = mp_obj_new_str_from_vstr(&current_vstr); \
            current_vstr_active = false; \
        } else if (current_str != MP_OBJ_NULL) { \
            out_str = current_str; \
        } else { \
            out_str = MP_OBJ_NEW_QSTR(MP_QSTR_); \
        } \
        strings_tuple->items[string_idx++] = out_str; \
        current_str = MP_OBJ_NULL; \
    } while (0)

            for (size_t i = 0; i < n_args; i++) {
                mp_obj_t arg = args[i];
                if (mp_obj_is_exact_type(arg, &mp_type_interpolation)) {
                    FLUSH_CURRENT_STRING();
                    interpolations_tuple->items[interp_idx++] = arg;
                } else {
                    size_t str_len;
                    const char *str_data = mp_obj_str_get_data(arg, &str_len);
                    if (current_vstr_active) {
                        vstr_add_strn(&current_vstr, str_data, str_len);
                    } else if (current_str == MP_OBJ_NULL) {
                        current_str = arg;
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

            FLUSH_CURRENT_STRING();

#undef FLUSH_CURRENT_STRING

            strings_obj = MP_OBJ_FROM_PTR(strings_tuple);
            interpolations_obj = MP_OBJ_FROM_PTR(interpolations_tuple);
        }
    }

    mp_obj_template_t *self = m_new_obj(mp_obj_template_t);
    self->base.type = type;
    self->strings = strings_obj;
    self->interpolations = interpolations_obj;
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t template_str(mp_obj_t self_in) {
    // Same as __repr__(), following CPython behavior
    mp_print_t print;
    vstr_t vstr;
    vstr_init_print(&vstr, 16, &print);
    template_print(&print, self_in, PRINT_STR);
    return mp_obj_new_str_from_vstr(&vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(template_str_obj, template_str);

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

    while (self->index < strings->len || (self->index < interps->len && self->at_interpolation)) {
        if (!self->at_interpolation) {
            if (self->index < strings->len) {
                mp_obj_t str_obj = strings->items[self->index];
                size_t str_len;
                mp_obj_str_get_data(str_obj, &str_len);

                if (str_len > 0) {
                    self->at_interpolation = true;
                    return str_obj;
                } else {
                    self->at_interpolation = true;
                }
            }
        }

        if (self->at_interpolation) {
            if (self->index < interps->len) {
                mp_obj_t interp = interps->items[self->index];
                self->index++;
                self->at_interpolation = false;
                return interp;
            } else {
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

static mp_obj_t template_iter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_template_iter_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_template_iter_t *iter = (mp_obj_template_iter_t *)iter_buf;
    iter->base.type = &mp_type_template_iter;
    iter->template = self_in;
    iter->index = 0;
    iter->at_interpolation = false;
    return MP_OBJ_FROM_PTR(iter);
}

static void template_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_template_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
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
            dest[0] = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&template_str_obj), self_in);
        }
    }
}

static mp_obj_t template_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_template_t *lhs = MP_OBJ_TO_PTR(lhs_in);

    switch (op) {
        case MP_BINARY_OP_ADD: {
            if (!mp_obj_is_exact_type(rhs_in, &mp_type_template)) {
                if (mp_obj_is_str(rhs_in)) {
                    mp_raise_TypeError(MP_ERROR_TEXT("can only concatenate string.templatelib.Template (not \"str\") to string.templatelib.Template"));
                } else {
                    mp_raise_msg_varg(&mp_type_TypeError,
                        MP_ERROR_TEXT("can only concatenate string.templatelib.Template (not \"%s\") to string.templatelib.Template"),
                        mp_obj_get_type_str(rhs_in));
                }
                return MP_OBJ_NULL;
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

        case MP_BINARY_OP_REVERSE_ADD: {
            if (mp_obj_is_str(rhs_in)) {
                mp_raise_TypeError(MP_ERROR_TEXT("can only concatenate str (not \"string.templatelib.Template\") to str"));
            }
            return MP_OBJ_NULL;
        }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

static const mp_rom_map_elem_t template_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___str__), MP_ROM_PTR(&template_str_obj) },
};
static MP_DEFINE_CONST_DICT(template_locals_dict, template_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_template,
    MP_QSTR_Template,
    MP_TYPE_FLAG_NONE,
    make_new, template_make_new,
    print, template_print,
    attr, template_attr,
    iter, template_iter,
    binary_op, template_binary_op,
    locals_dict, &template_locals_dict
    );

static mp_obj_t mp_builtin___template__(mp_obj_t strings, mp_obj_t interpolations_in) {
    if (!mp_obj_is_type(strings, &mp_type_tuple)) {
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("__template__ strings must be tuple, got '%s'"),
            mp_obj_get_type_str(strings));
    }

    mp_obj_t *strings_items;
    size_t strings_len;
    mp_obj_tuple_get(strings, &strings_len, &strings_items);

    if (strings_len == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("__template__ strings must be non-empty"));
    }

    if (strings_len > TEMPLATE_COUNT_MAX) {
        mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("template string too large for header format"));
    }

    for (size_t i = 0; i < strings_len; i++) {
        if (!mp_obj_is_str(strings_items[i])) {
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("__template__ strings must contain only str, got '%s' at index %d"),
                mp_obj_get_type_str(strings_items[i]), i);
        }
    }

    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(interpolations_in, &len, &items);

    if (len > TEMPLATE_COUNT_MAX) {
        mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("template string too large for header format"));
    }

    mp_obj_tuple_t *interpolations_tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, len, &mp_type_tuple);
    interpolations_tuple->len = len;

    for (size_t i = 0; i < len; i++) {
        mp_obj_t *interp_items;
        size_t interp_len;
        mp_obj_get_array(items[i], &interp_len, &interp_items);

        if (interp_len >= 2) {
            mp_obj_t value = interp_items[0];
            mp_obj_t expression = interp_items[1];
            mp_obj_t conversion = (interp_len > 2) ? interp_items[2] : mp_const_none;
            mp_obj_t format_spec = (interp_len > 3) ? interp_items[3] : MP_OBJ_NEW_QSTR(MP_QSTR_);

            interpolations_tuple->items[i] = mp_obj_new_interpolation(value, expression, conversion, format_spec);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid interpolation format"));
        }
    }

    if (strings_len != interpolations_tuple->len + 1) {
        // Shortened message for all builds to avoid stack overflow during ROM compression on constrained platforms (Windows x86, ASan)
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("__template__ count mismatch: strings=%d, interpolations=%d"),
            strings_len, interpolations_tuple->len);
    }

    mp_obj_template_t *self = mp_obj_malloc(mp_obj_template_t, &mp_type_template);
    self->strings = strings;
    self->interpolations = MP_OBJ_FROM_PTR(interpolations_tuple);
    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin___template___obj, mp_builtin___template__);

static const mp_rom_map_elem_t mp_module__tstring_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__tstring) },
    { MP_ROM_QSTR(MP_QSTR_Template), MP_ROM_PTR(&mp_type_template) },
    { MP_ROM_QSTR(MP_QSTR_Interpolation), MP_ROM_PTR(&mp_type_interpolation) },
};

static MP_DEFINE_CONST_DICT(mp_module__tstring_globals, mp_module__tstring_globals_table);

const mp_obj_module_t mp_module__tstring = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module__tstring_globals,
};

MP_REGISTER_MODULE(MP_QSTR__tstring, mp_module__tstring);

#endif
