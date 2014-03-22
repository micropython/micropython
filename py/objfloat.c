#include <stdlib.h>
#include <assert.h>
#include <math.h>

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

STATIC void float_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_float_t *o = o_in;
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    char buf[32];
    format_float(o->value, buf, sizeof(buf), 'g', 6, '\0');
    print(env, "%s", buf);
#else
    print(env, "%.8g", (double) o->value);
#endif
}

STATIC mp_obj_t float_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            return mp_obj_new_float(0);

        case 1:
            if (MP_OBJ_IS_STR(args[0])) {
                // a string, parse it
                uint l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_decimal(s, l, false, false);
            } else if (MP_OBJ_IS_TYPE(args[0], &mp_type_float)) {
                // a float, just return it
                return args[0];
            } else {
                // something else, try to cast it to a float
                return mp_obj_new_float(mp_obj_get_float(args[0]));
            }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "float takes at most 1 argument, %d given", n_args));
    }
}

STATIC mp_obj_t float_unary_op(int op, mp_obj_t o_in) {
    mp_obj_float_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_BOOL: return MP_BOOL(o->value != 0);
        case RT_UNARY_OP_POSITIVE: return o_in;
        case RT_UNARY_OP_NEGATIVE: return mp_obj_new_float(-o->value);
        default: return NULL; // op not supported
    }
}

STATIC mp_obj_t float_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_float_t *lhs = lhs_in;
    if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_complex)) {
        return mp_obj_complex_binary_op(op, lhs->value, 0, rhs_in);
    } else {
        return mp_obj_float_binary_op(op, lhs->value, rhs_in);
    }
}

const mp_obj_type_t mp_type_float = {
    { &mp_type_type },
    .name = MP_QSTR_float,
    .print = float_print,
    .make_new = float_make_new,
    .unary_op = float_unary_op,
    .binary_op = float_binary_op,
};

mp_obj_t mp_obj_new_float(mp_float_t value) {
    mp_obj_float_t *o = m_new(mp_obj_float_t, 1);
    o->base.type = &mp_type_float;
    o->value = value;
    return (mp_obj_t)o;
}

mp_float_t mp_obj_float_get(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_float));
    mp_obj_float_t *self = self_in;
    return self->value;
}

mp_obj_t mp_obj_float_binary_op(int op, mp_float_t lhs_val, mp_obj_t rhs_in) {
    mp_float_t rhs_val = mp_obj_get_float(rhs_in); // can be any type, this function will convert to float (if possible)
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
        case RT_BINARY_OP_INPLACE_TRUE_DIVIDE: 
            lhs_val /= rhs_val; 
            if (isinf(lhs_val)){ // check for division by zero
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ZeroDivisionError, "float division by zero"));
            }
            break;
        case RT_BINARY_OP_LESS: return MP_BOOL(lhs_val < rhs_val);
        case RT_BINARY_OP_MORE: return MP_BOOL(lhs_val > rhs_val);
        case RT_BINARY_OP_LESS_EQUAL: return MP_BOOL(lhs_val <= rhs_val);
        case RT_BINARY_OP_MORE_EQUAL: return MP_BOOL(lhs_val >= rhs_val);

        return NULL; // op not supported
    }
    return mp_obj_new_float(lhs_val);
}

#endif // MICROPY_ENABLE_FLOAT
