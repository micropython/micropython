#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
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

void complex_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_complex_t *o = o_in;
    if (o->real == 0) {
        print(env, "%.8gj", o->imag);
    } else {
        print(env, "(%.8g+%.8gj)", o->real, o->imag);
    }
}

mp_obj_t complex_unary_op(int op, mp_obj_t o_in) {
    mp_obj_complex_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_NOT: if (o->real != 0 || o->imag != 0) { return mp_const_true;} else { return mp_const_false; }
        case RT_UNARY_OP_POSITIVE: return o_in;
        case RT_UNARY_OP_NEGATIVE: return mp_obj_new_complex(-o->real, -o->imag);
        default: return MP_OBJ_NULL; // op not supported
    }
}

mp_obj_t complex_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_float_t lhs_real, lhs_imag, rhs_real, rhs_imag;
    mp_obj_complex_get(lhs_in, &lhs_real, &lhs_imag);
    mp_obj_complex_get(rhs_in, &rhs_real, &rhs_imag);
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

const mp_obj_type_t complex_type = {
    { &mp_const_type },
    "complex",
    complex_print, // print
    NULL, // call_n
    complex_unary_op, // unary_op
    complex_binary_op, // binary_op
    NULL, // getiter
    NULL, // iternext
    { { NULL, NULL }, }, // method list
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

#endif
