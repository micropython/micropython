#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "parsenum.h"
#include "runtime0.h"

#if MICROPY_ENABLE_FLOAT

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#include "formatfloat.h"
#endif

typedef struct _mp_obj_complex_t {
    mp_obj_base_t base;
    mp_float_t real;
    mp_float_t imag;
} mp_obj_complex_t;

mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);

STATIC void complex_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_complex_t *o = o_in;
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    char buf[32];
    if (o->real == 0) {
        format_float(o->imag, buf, sizeof(buf), 'g', 6, '\0');
        print(env, "%sj", buf);
    } else {
        format_float(o->real, buf, sizeof(buf), 'g', 6, '\0');
        print(env, "(%s+", buf);
        format_float(o->imag, buf, sizeof(buf), 'g', 6, '\0');
        print(env, "%sj)", buf);
    }
#else
    if (o->real == 0) {
        print(env, "%.8gj",  (double) o->imag);
    } else {
        print(env, "(%.8g+%.8gj)", (double) o->real, (double) o->imag);
    }
#endif
}

STATIC mp_obj_t complex_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            return mp_obj_new_complex(0, 0);

        case 1:
            if (MP_OBJ_IS_STR(args[0])) {
                // a string, parse it
                uint l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_decimal(s, l, true, true);
            } else if (MP_OBJ_IS_TYPE(args[0], &mp_type_complex)) {
                // a complex, just return it
                return args[0];
            } else {
                // something else, try to cast it to a complex
                return mp_obj_new_complex(mp_obj_get_float(args[0]), 0);
            }

        case 2: {
            mp_float_t real, imag;
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_complex)) {
                mp_obj_complex_get(args[0], &real, &imag);
            } else {
                real = mp_obj_get_float(args[0]);
                imag = 0;
            }
            if (MP_OBJ_IS_TYPE(args[1], &mp_type_complex)) {
                mp_float_t real2, imag2;
                mp_obj_complex_get(args[1], &real2, &imag2);
                real -= imag2;
                imag += real2;
            } else {
                imag += mp_obj_get_float(args[1]);
            }
            return mp_obj_new_complex(real, imag);
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "complex takes at most 2 arguments, %d given", n_args));
    }
}

STATIC mp_obj_t complex_unary_op(int op, mp_obj_t o_in) {
    mp_obj_complex_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(o->real != 0 || o->imag != 0);
        case MP_UNARY_OP_POSITIVE: return o_in;
        case MP_UNARY_OP_NEGATIVE: return mp_obj_new_complex(-o->real, -o->imag);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t complex_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_complex_t *lhs = lhs_in;
    return mp_obj_complex_binary_op(op, lhs->real, lhs->imag, rhs_in);
}

const mp_obj_type_t mp_type_complex = {
    { &mp_type_type },
    .name = MP_QSTR_complex,
    .print = complex_print,
    .make_new = complex_make_new,
    .unary_op = complex_unary_op,
    .binary_op = complex_binary_op,
};

mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag) {
    mp_obj_complex_t *o = m_new_obj(mp_obj_complex_t);
    o->base.type = &mp_type_complex;
    o->real = real;
    o->imag = imag;
    return o;
}

void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_complex));
    mp_obj_complex_t *self = self_in;
    *real = self->real;
    *imag = self->imag;
}

mp_obj_t mp_obj_complex_binary_op(int op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in) {
    mp_float_t rhs_real, rhs_imag;
    mp_obj_get_complex(rhs_in, &rhs_real, &rhs_imag); // can be any type, this function will convert to float (if possible)
    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
            lhs_real += rhs_real;
            lhs_imag += rhs_imag;
            break;
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_INPLACE_SUBTRACT:
            lhs_real -= rhs_real;
            lhs_imag -= rhs_imag;
            break;
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY:
        {
            mp_float_t real = lhs_real * rhs_real - lhs_imag * rhs_imag;
            lhs_imag = lhs_real * rhs_imag + lhs_imag * rhs_real;
            lhs_real = real;
            break;
        }
        /* TODO floor(?) the value
        case MP_BINARY_OP_FLOOR_DIVIDE:
        case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE: val = lhs_val / rhs_val; break;
        */
        /* TODO
        case MP_BINARY_OP_TRUE_DIVIDE:
        case MP_BINARY_OP_INPLACE_TRUE_DIVIDE: val = lhs_val / rhs_val; break;
        */
        return NULL; // op not supported
    }
    return mp_obj_new_complex(lhs_real, lhs_imag);
}

#endif
