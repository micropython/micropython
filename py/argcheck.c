/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "py/runtime.h"

#include "supervisor/shared/translate.h"

void mp_arg_check_num_sig(size_t n_args, size_t n_kw, uint32_t sig) {
    // TODO maybe take the function name as an argument so we can print nicer error messages

    // The reverse of MP_OBJ_FUN_MAKE_SIG
    bool takes_kw = sig & 1;
    size_t n_args_min = sig >> 17;
    size_t n_args_max = (sig >> 1) & 0xffff;

    if (n_kw && !takes_kw) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_arg_error_terse_mismatch();
        #else
        mp_raise_TypeError(MP_ERROR_TEXT("function doesn't take keyword arguments"));
        #endif
    }

    if (n_args_min == n_args_max) {
        if (n_args != n_args_min) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_TypeError_varg(MP_ERROR_TEXT("function takes %d positional arguments but %d were given"),
                n_args_min, n_args);
            #endif
        }
    } else {
        if (n_args < n_args_min) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_TypeError_varg(
                MP_ERROR_TEXT("function missing %d required positional arguments"),
                n_args_min - n_args);
            #endif
        } else if (n_args > n_args_max) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_TypeError_varg(
                MP_ERROR_TEXT("function expected at most %d arguments, got %d"),
                n_args_max, n_args);
            #endif
        }
    }
}

inline void mp_arg_check_num(size_t n_args, mp_map_t *kw_args, size_t n_args_min, size_t n_args_max, bool takes_kw) {
    size_t n_kw = 0;
    if (kw_args != NULL) {
        n_kw = kw_args->used;
    }
    mp_arg_check_num_sig(n_args, n_kw, MP_OBJ_FUN_MAKE_SIG(n_args_min, n_args_max, takes_kw));
}

inline void mp_arg_check_num_kw_array(size_t n_args, size_t n_kw, size_t n_args_min, size_t n_args_max, bool takes_kw) {
    mp_arg_check_num_sig(n_args, n_kw, MP_OBJ_FUN_MAKE_SIG(n_args_min, n_args_max, takes_kw));
}

void mp_arg_parse_all(size_t n_pos, const mp_obj_t *pos, mp_map_t *kws, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals) {
    size_t pos_found = 0, kws_found = 0;
    for (size_t i = 0; i < n_allowed; i++) {
        mp_obj_t given_arg;
        if (i < n_pos) {
            if (allowed[i].flags & MP_ARG_KW_ONLY) {
                goto extra_positional;
            }
            pos_found++;
            given_arg = pos[i];
        } else {
            mp_map_elem_t *kw = NULL;
            if (kws != NULL) {
                kw = mp_map_lookup(kws, MP_OBJ_NEW_QSTR(allowed[i].qst), MP_MAP_LOOKUP);
            }
            if (kw == NULL) {
                if (allowed[i].flags & MP_ARG_REQUIRED) {
                    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
                    mp_arg_error_terse_mismatch();
                    #else
                    mp_raise_TypeError_varg(MP_ERROR_TEXT("'%q' argument required"), allowed[i].qst);
                    #endif
                }
                out_vals[i] = allowed[i].defval;
                continue;
            } else {
                kws_found++;
                given_arg = kw->value;
            }
        }
        if ((allowed[i].flags & MP_ARG_KIND_MASK) == MP_ARG_BOOL) {
            out_vals[i].u_bool = mp_obj_is_true(given_arg);
        } else if ((allowed[i].flags & MP_ARG_KIND_MASK) == MP_ARG_INT) {
            out_vals[i].u_int = mp_obj_get_int(given_arg);
        } else {
            assert((allowed[i].flags & MP_ARG_KIND_MASK) == MP_ARG_OBJ);
            out_vals[i].u_obj = given_arg;
        }
    }
    if (pos_found < n_pos) {
    extra_positional:
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_arg_error_terse_mismatch();
        #else
        // TODO better error message
        mp_raise_TypeError(MP_ERROR_TEXT("extra positional arguments given"));
        #endif
    }
    if (kws != NULL && kws_found < kws->used) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_arg_error_terse_mismatch();
        #else
        // TODO better error message
        mp_raise_TypeError(MP_ERROR_TEXT("extra keyword arguments given"));
        #endif
    }
}

void mp_arg_parse_all_kw_array(size_t n_pos, size_t n_kw, const mp_obj_t *args, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals) {
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_pos);
    mp_arg_parse_all(n_pos, args, &kw_args, n_allowed, allowed, out_vals);
}

NORETURN void mp_arg_error_terse_mismatch(void) {
    mp_raise_TypeError(MP_ERROR_TEXT("argument num/types mismatch"));
}

#if MICROPY_CPYTHON_COMPAT
NORETURN void mp_arg_error_unimpl_kw(void) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("keyword argument(s) not yet implemented - use normal args instead"));
}
#endif


mp_int_t mp_arg_validate_int_min(mp_int_t i, mp_int_t min, qstr arg_name) {
    if (i < min) {
        mp_raise_ValueError_varg(translate("%q must be >= %d"), arg_name, min);
    }
    return i;
}

mp_int_t mp_arg_validate_int_max(mp_int_t i, mp_int_t max, qstr arg_name) {
    if (i > max) {
        mp_raise_ValueError_varg(translate("%q must <= %d"), arg_name, max);
    }
    return i;
}

mp_int_t mp_arg_validate_int_range(mp_int_t i, mp_int_t min, mp_int_t max, qstr arg_name) {
    if (i < min || i > max) {
        mp_raise_ValueError_varg(translate("%q must be %d-%d"), arg_name, min, max);
    }
    return i;
}

mp_float_t mp_arg_validate_obj_float_non_negative(mp_obj_t float_in, mp_float_t default_for_null, qstr arg_name) {
    const mp_float_t f = (float_in == MP_OBJ_NULL)
        ? default_for_null
        : mp_obj_get_float(float_in);
    if (f <= (mp_float_t)0.0) {
        mp_raise_ValueError_varg(translate("%q must be >= 0"), arg_name);
    }
    return f;
}

size_t mp_arg_validate_length_with_name(mp_int_t i, size_t length, qstr arg_name, qstr length_name) {
    if (i != (mp_int_t)length) {
        mp_raise_ValueError_varg(translate("%q length must be %q"), MP_QSTR_pressed, MP_QSTR_num_keys);
    }
    return (size_t)i;
}

mp_obj_t mp_arg_validate_type(mp_obj_t obj, const mp_obj_type_t *type, qstr arg_name) {
    if (!mp_obj_is_type(obj, type)) {
        mp_raise_TypeError_varg(translate("%q must of type %q"), arg_name, type->name);
    }
    return obj;
}

mp_obj_t mp_arg_validate_string(mp_obj_t obj, qstr arg_name) {
    if (!mp_obj_is_str(obj)) {
        mp_raise_TypeError_varg(translate("%q must be a string"), arg_name);
    }
    return obj;
}
