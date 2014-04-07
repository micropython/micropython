#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "parsenum.h"
#include "mpz.h"
#include "objint.h"
#include "runtime0.h"
#include "runtime.h"

#if MICROPY_ENABLE_FLOAT
#include <math.h>
#endif

// This dispatcher function is expected to be independent of the implementation of long int
STATIC mp_obj_t mp_obj_int_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
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
#if MICROPY_ENABLE_FLOAT
            } else if (MP_OBJ_IS_TYPE(args[0], &mp_type_float)) {
                return MP_OBJ_NEW_SMALL_INT((machine_int_t)(MICROPY_FLOAT_C_FUN(trunc)(mp_obj_float_get(args[0]))));
#endif
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
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "int takes at most 2 arguments, %d given", n_args));
    }
}

void mp_obj_int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    // The size of this buffer is rather arbitrary. If it's not large
    // enough, a dynamic one will be allocated.
    char stack_buf[sizeof(machine_int_t) * 4];
    char *buf = stack_buf;
    int buf_size = sizeof(stack_buf);
    int fmt_size;

    char *str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, self_in, 10, NULL, '\0', '\0');
    print(env, "%s", str);

    if (buf != stack_buf) {
        m_free(buf, buf_size);
    }
}

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE || MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG
typedef mp_longint_impl_t   fmt_int_t;
#else
typedef mp_small_int_t   fmt_int_t;
#endif

static const uint log_base2_floor[] = {
    0,
    0, 1, 1, 2,
    2, 2, 2, 3,
    3, 3, 3, 3,
    3, 3, 3, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 5
};

uint int_as_str_size_formatted(uint base, const char *prefix, char comma) {
    if (base < 2 || base > 32) {
        return 0;
    }

    uint num_digits = sizeof(fmt_int_t) * 8 / log_base2_floor[base] + 1;
    uint num_commas = comma ? num_digits / 3: 0;
    uint prefix_len = prefix ? strlen(prefix) : 0;
    return num_digits + num_commas + prefix_len + 2; // +1 for sign, +1 for null byte
}

// This routine expects you to pass in a buffer and size (in *buf and buf_size).
// If, for some reason, this buffer is too small, then it will allocate a
// buffer and return the allocated buffer and size in *buf and *buf_size. It
// is the callers responsibility to free this allocated buffer.
//
// The resulting formatted string will be returned from this function and the
// formatted size will be in *fmt_size.
char *mp_obj_int_formatted(char **buf, int *buf_size, int *fmt_size, mp_obj_t self_in,
                           int base, const char *prefix, char base_char, char comma) {
    if (!MP_OBJ_IS_INT(self_in)) {
        buf[0] = '\0';
        *fmt_size = 0;
        return *buf;
    }
    fmt_int_t num = mp_obj_get_int(self_in);
    char sign = '\0';
    if (num < 0) {
        num = -num;
        sign = '-';
    }

    uint needed_size = int_as_str_size_formatted(base, prefix, comma);
    if (needed_size > *buf_size) {
        *buf = m_new(char, needed_size);
        *buf_size = needed_size;
    }
    char *str = *buf;

    char *b = str + needed_size;
    *(--b) = '\0';
    char *last_comma = b;

    if (num == 0) {
        *(--b) = '0';
    } else {
        do {
            int c = num % base;
            num /= base;
            if (c >= 10) {
                c += base_char - 10;
            } else {
                c += '0';
            }
            *(--b) = c;
            if (comma && num != 0 && b > str && (last_comma - b) == 3) {
                *(--b) = comma;
                last_comma = b;
            }
        }
        while (b > str && num != 0);
    }
    if (prefix) {
        size_t prefix_len = strlen(prefix);
        char *p = b - prefix_len;
        if (p > str) {
            b = p;
            while (*prefix) {
                *p++ = *prefix++;
            }
        }
    }
    if (sign && b > str) {
        *(--b) = sign;
    }
    *fmt_size = *buf + needed_size - b - 1;

    return b;
}

bool mp_obj_int_is_positive(mp_obj_t self_in) {
    return mp_obj_get_int(self_in) >= 0;
}
#endif  // LONGLONG or NONE

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE

// This is called for operations on SMALL_INT that are not handled by mp_unary_op
mp_obj_t mp_obj_int_unary_op(int op, mp_obj_t o_in) {
    return MP_OBJ_NULL;
}

// This is called for operations on SMALL_INT that are not handled by mp_binary_op
mp_obj_t mp_obj_int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    return mp_obj_int_binary_op_extra_cases(op, lhs_in, rhs_in);
}

// This is called only with strings whose value doesn't fit in SMALL_INT
mp_obj_t mp_obj_new_int_from_long_str(const char *s) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OverflowError, "long int not supported in this build"));
    return mp_const_none;
}

// This is called when an integer larger than a SMALL_INT is needed (although val might still fit in a SMALL_INT)
mp_obj_t mp_obj_new_int_from_ll(long long val) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
    return mp_const_none;
}

mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value) {
    // SMALL_INT accepts only signed numbers, of one bit less size
    // then word size, which totals 2 bits less for unsigned numbers.
    if ((value & (WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
    return mp_const_none;
}

mp_obj_t mp_obj_new_int(machine_int_t value) {
    if (MP_OBJ_FITS_SMALL_INT(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OverflowError, "small int overflow"));
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

// This dispatcher function is expected to be independent of the implementation of long int
// It handles the extra cases for integer-like arithmetic
mp_obj_t mp_obj_int_binary_op_extra_cases(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (rhs_in == mp_const_false) {
        // false acts as 0
        return mp_binary_op(op, lhs_in, MP_OBJ_NEW_SMALL_INT(0));
    } else if (rhs_in == mp_const_true) {
        // true acts as 0
        return mp_binary_op(op, lhs_in, MP_OBJ_NEW_SMALL_INT(1));
    } else if (op == MP_BINARY_OP_MULTIPLY) {
        if (MP_OBJ_IS_STR(rhs_in) || MP_OBJ_IS_TYPE(rhs_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(rhs_in, &mp_type_list)) {
            // multiply is commutative for these types, so delegate to them
            return mp_binary_op(op, rhs_in, lhs_in);
        }
    }
    return MP_OBJ_NULL;
}

const mp_obj_type_t mp_type_int = {
    { &mp_type_type },
    .name = MP_QSTR_int,
    .print = mp_obj_int_print,
    .make_new = mp_obj_int_make_new,
    .unary_op = mp_obj_int_unary_op,
    .binary_op = mp_obj_int_binary_op,
};
