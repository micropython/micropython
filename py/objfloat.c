#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"

#if MICROPY_ENABLE_FLOAT

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

mp_obj_t mp_obj_new_float(mp_float_t value);

void float_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_float_t *o = o_in;
    print(env, "%.8g", o->value);
}

mp_obj_t float_unary_op(int op, mp_obj_t o_in) {
    mp_obj_float_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_NOT: if (o->value != 0) { return mp_const_true;} else { return mp_const_false; }
        case RT_UNARY_OP_POSITIVE: return o_in;
        case RT_UNARY_OP_NEGATIVE: return mp_obj_new_float(-o->value);
        default: return NULL; // op not supported
    }
}

mp_obj_t float_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (MP_OBJ_IS_TYPE(rhs_in, &complex_type)) {
        return complex_type.binary_op(op, lhs_in, rhs_in);
    }
    mp_float_t lhs_val = mp_obj_get_float(lhs_in);
    mp_float_t rhs_val = mp_obj_get_float(rhs_in);
    switch (op) {
        case RT_BINARY_OP_ADD:
        case RT_BINARY_OP_INPLACE_ADD: lhs_val += rhs_val; break;
        case RT_BINARY_OP_SUBTRACT:
        case RT_BINARY_OP_INPLACE_SUBTRACT: lhs_val -= rhs_val; break;
        case RT_BINARY_OP_MULTIPLY:
        case RT_BINARY_OP_INPLACE_MULTIPLY: lhs_val *= rhs_val; break;
        /* TODO floor(?) the value
        case RT_BINARY_OP_FLOOR_DIVIDE:
        case RT_BINARY_OP_INPLACE_FLOOR_DIVIDE: val = lhs_val / rhs_val; break;
        */
        case RT_BINARY_OP_TRUE_DIVIDE:
        case RT_BINARY_OP_INPLACE_TRUE_DIVIDE: lhs_val /= rhs_val; break;
        return NULL; // op not supported
    }
    return mp_obj_new_float(lhs_val);
}

const mp_obj_type_t float_type = {
    { &mp_const_type },
    "float",
    float_print,
    NULL, // call_n
    float_unary_op,
    float_binary_op,
    NULL, // getiter
    NULL, // iternext
    { { NULL, NULL }, }, // method list
};

mp_obj_t mp_obj_new_float(mp_float_t value) {
    mp_obj_float_t *o = m_new(mp_obj_float_t, 1);
    o->base.type = &float_type;
    o->value = value;
    return (mp_obj_t)o;
}

mp_float_t mp_obj_float_get(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &float_type));
    mp_obj_float_t *self = self_in;
    return self->value;
}

#endif
