/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include <assert.h>

#include "py/nlr.h"
#include "py/runtime.h"

void mp_arg_check_num(size_t n_args, size_t n_kw, size_t n_args_min, size_t n_args_max, bool takes_kw) {
    // TODO maybe take the function name as an argument so we can print nicer error messages

    if (n_kw && !takes_kw) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_arg_error_terse_mismatch();
        } else {
            mp_raise_TypeError("function does not take keyword arguments");
        }
    }

    if (n_args_min == n_args_max) {
        if (n_args != n_args_min) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_arg_error_terse_mismatch();
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function takes %d positional arguments but %d were given",
                    n_args_min, n_args));
            }
        }
    } else {
        if (n_args < n_args_min) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_arg_error_terse_mismatch();
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function missing %d required positional arguments",
                    n_args_min - n_args));
            }
        } else if (n_args > n_args_max) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_arg_error_terse_mismatch();
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function expected at most %d arguments, got %d",
                    n_args_max, n_args));
            }
        }
    }
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
            mp_map_elem_t *kw = mp_map_lookup(kws, MP_OBJ_NEW_QSTR(allowed[i].qst), MP_MAP_LOOKUP);
            if (kw == NULL) {
                if (allowed[i].flags & MP_ARG_REQUIRED) {
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        mp_arg_error_terse_mismatch();
                    } else {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                            "'%q' argument required", allowed[i].qst));
                    }
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
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_arg_error_terse_mismatch();
        } else {
            // TODO better error message
            mp_raise_TypeError("extra positional arguments given");
        }
    }
    if (kws_found < kws->used) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_arg_error_terse_mismatch();
        } else {
            // TODO better error message
            mp_raise_TypeError("extra keyword arguments given");
        }
    }
}

void mp_arg_parse_all_kw_array(size_t n_pos, size_t n_kw, const mp_obj_t *args, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals) {
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_pos);
    mp_arg_parse_all(n_pos, args, &kw_args, n_allowed, allowed, out_vals);
}

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE || _MSC_VER
NORETURN void mp_arg_error_terse_mismatch(void) {
    mp_raise_TypeError("argument num/types mismatch");
}
#endif

#if MICROPY_CPYTHON_COMPAT
NORETURN void mp_arg_error_unimpl_kw(void) {
    mp_not_implemented("keyword argument(s) not yet implemented - use normal args instead");
}
#endif
