#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime0.h"
#include "map.h"

#if MICROPY_ENABLE_FLOAT

typedef struct _mp_obj_complex_t {
    mp_obj_base_t base;
    mp_float_t real;
    mp_float_t imag;
} mp_obj_complex_t;

mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);

void complex_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_complex_t *o = o_in;
    if (o->real == 0) {
        print(env, "%.8gj", o->imag);
    } else {
        print(env, "(%.8g+%.8gj)", o->real, o->imag);
    }
}

// args are reverse in the array
static mp_obj_t complex_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0:
            return mp_obj_new_complex(0, 0);

        case 1:
            // TODO allow string as first arg and parse it
            if (MP_OBJ_IS_TYPE(args[0], &complex_type)) {
                return args[0];
            } else {
                return mp_obj_new_complex(mp_obj_get_float(args[0]), 0);
            }

        case 2:
        {
            mp_float_t real, imag;
            if (MP_OBJ_IS_TYPE(args[1], &complex_type)) {
                mp_obj_complex_get(args[1], &real, &imag);
            } else {
                real = mp_obj_get_float(args[1]);
                imag = 0;
            }
            if (MP_OBJ_IS_TYPE(args[0], &complex_type)) {
                mp_float_t real2, imag2;
                mp_obj_complex_get(args[0], &real2, &imag2);
                real -= imag2;
                imag += real2;
            } else {
                imag += mp_obj_get_float(args[0]);
            }
            return mp_obj_new_complex(real, imag);
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "complex takes at most 2 arguments, %d given", (void*)(machine_int_t)n_args));
    }
}

static mp_obj_t complex_unary_op(int op, mp_obj_t o_in) {
    mp_obj_complex_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_NOT: if (o->real != 0 || o->imag != 0) { return mp_const_true;} else { return mp_const_false; }
        case RT_UNARY_OP_POSITIVE: return o_in;
        case RT_UNARY_OP_NEGATIVE: return mp_obj_new_complex(-o->real, -o->imag);
        default: return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t complex_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in, ...) {
    mp_obj_complex_t *lhs = lhs_in;
    return mp_obj_complex_binary_op(op, lhs->real, lhs->imag, rhs_in);
}

const mp_obj_type_t complex_type = {
    { &mp_const_type },
    "complex",
    .print = complex_print,
    .make_new = complex_make_new,
    .unary_op = complex_unary_op,
    .binary_op = complex_binary_op,
};

mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag) {
    mp_obj_complex_t *o = m_new_obj(mp_obj_complex_t);
    o->base.type = &complex_type;
    o->real = real;
    o->imag = imag;
    return o;
}

void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag) {
    assert(MP_OBJ_IS_TYPE(self_in, &complex_type));
    mp_obj_complex_t *self = self_in;
    *real = self->real;
    *imag = self->imag;
}

mp_obj_t mp_obj_complex_binary_op(int op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in) {
    mp_float_t rhs_real, rhs_imag;
    mp_obj_get_complex(rhs_in, &rhs_real, &rhs_imag); // can be any type, this function will convert to float (if possible)
    switch (op) {
        case RT_BINARY_OP_ADD:
        case RT_BINARY_OP_INPLACE_ADD:
            lhs_real += rhs_real;
            lhs_imag += rhs_imag;
            break;
        case RT_BINARY_OP_SUBTRACT:
        case RT_BINARY_OP_INPLACE_SUBTRACT:
            lhs_real -= rhs_real;
            lhs_imag -= rhs_imag;
            break;
        case RT_BINARY_OP_MULTIPLY:
        case RT_BINARY_OP_INPLACE_MULTIPLY:
        {
            mp_float_t real = lhs_real * rhs_real - lhs_imag * rhs_imag;
            lhs_imag = lhs_real * rhs_imag + lhs_imag * rhs_real;
            lhs_real = real;
            break;
        }
        /* TODO floor(?) the value
        case RT_BINARY_OP_FLOOR_DIVIDE:
        case RT_BINARY_OP_INPLACE_FLOOR_DIVIDE: val = lhs_val / rhs_val; break;
        */
        /* TODO
        case RT_BINARY_OP_TRUE_DIVIDE:
        case RT_BINARY_OP_INPLACE_TRUE_DIVIDE: val = lhs_val / rhs_val; break;
        */
        return NULL; // op not supported
    }
    return mp_obj_new_complex(lhs_real, lhs_imag);
}

#endif
