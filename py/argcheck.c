#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

void mp_arg_check_num(uint n_args, uint n_kw, uint n_args_min, uint n_args_max, bool takes_kw) {
    // TODO maybe take the function name as an argument so we can print nicer error messages

    if (n_kw && !takes_kw) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "function does not take keyword arguments"));
    }

    if (n_args_min == n_args_max) {
        if (n_args != n_args_min) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                                                    "function takes %d positional arguments but %d were given",
                                                    n_args_min, n_args));
        }
    } else {
        if (n_args < n_args_min) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                                                    "function missing %d required positional arguments",
                                                    n_args_min - n_args));
        } else if (n_args > n_args_max) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                                                    "function expected at most %d arguments, got %d",
                                                    n_args_max, n_args));
        }
    }
}

void mp_arg_parse_all(uint n_pos, const mp_obj_t *pos, mp_map_t *kws, uint n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals) {
    uint pos_found = 0, kws_found = 0;
    for (uint i = 0; i < n_allowed; i++) {
        mp_obj_t given_arg;
        if (i < n_pos) {
            if (allowed[i].flags & MP_ARG_KW_ONLY) {
                goto extra_positional;
            }
            pos_found++;
            given_arg = pos[i];
        } else {
            mp_map_elem_t *kw = mp_map_lookup(kws, MP_OBJ_NEW_QSTR(allowed[i].qstr), MP_MAP_LOOKUP);
            if (kw == NULL) {
                if (allowed[i].flags & MP_ARG_REQUIRED) {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' argument required", qstr_str(allowed[i].qstr)));
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
        } else if ((allowed[i].flags & MP_ARG_KIND_MASK) == MP_ARG_OBJ) {
            out_vals[i].u_obj = given_arg;
        } else {
            assert(0);
        }
    }
    if (pos_found < n_pos) {
        // TODO better error message
        extra_positional:
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "extra positional arguments given"));
    }
    if (kws_found < kws->used) {
        // TODO better error message
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "extra keyword arguments given"));
    }
}
