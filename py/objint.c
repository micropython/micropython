#include <stdint.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "parsenum.h"
#include "mpz.h"
#include "objint.h"

// This dispatcher function is expected to be independent of the implementation
// of long int
STATIC mp_obj_t int_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_SMALL_INT(0);

        case 1:
            if (MP_OBJ_IS_STR(args[0])) {
                // a string, parse it
                uint l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_integer(s, l, 0);
            } else {
                return MP_OBJ_NEW_SMALL_INT(mp_obj_get_int(args[0]));
            }

        case 2:
        {
            // should be a string, parse it
            // TODO proper error checking of argument types
            uint l;
            const char *s = mp_obj_str_get_data(args[0], &l);
            return mp_parse_num_integer(s, l, mp_obj_get_int(args[1]));
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "int takes at most 2 arguments, %d given", n_args));
    }
}

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE

void int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        print(env, INT_FMT, MP_OBJ_SMALL_INT_VALUE(self_in));
    }
}

// This is called only for non-SMALL_INT
mp_obj_t int_unary_op(int op, mp_obj_t o_in) {
    assert(0);
    return mp_const_none;
}

// This is called only for non-SMALL_INT
mp_obj_t int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    assert(0);
    return mp_const_none;
}

// This is called only with strings whose value doesn't fit in SMALL_INT
mp_obj_t mp_obj_new_int_from_long_str(const char *s) {
    nlr_jump(mp_obj_new_exception_msg(&mp_type_OverflowError, "long int not supported in this build"));
    return mp_const_none;
}

// This is called when an integer larger than a SMALL_INT is needed (although val might still fit in a SMALL_INT)
mp_obj_t mp_obj_new_int_from_ll(long long val) {
    nlr_jump(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
    return mp_const_none;
}

mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value) {
    // SMALL_INT accepts only signed numbers, of one bit less size
    // then word size, which totals 2 bits less for unsigned numbers.
    if ((value & (WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    nlr_jump(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
    return mp_const_none;
}

mp_obj_t mp_obj_new_int(machine_int_t value) {
    if (MP_OBJ_FITS_SMALL_INT(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    nlr_jump(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
    return mp_const_none;
}

machine_int_t mp_obj_int_get(mp_obj_t self_in) {
    return MP_OBJ_SMALL_INT_VALUE(self_in);
}

machine_int_t mp_obj_int_get_checked(mp_obj_t self_in) {
    return MP_OBJ_SMALL_INT_VALUE(self_in);
}

#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_int_as_float(mp_obj_t self_in) {
    return MP_OBJ_SMALL_INT_VALUE(self_in);
}
#endif

#endif // MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE

const mp_obj_type_t int_type = {
    { &mp_type_type },
    .name = MP_QSTR_int,
    .print = int_print,
    .make_new = int_make_new,
    .unary_op = int_unary_op,
    .binary_op = int_binary_op,
};
