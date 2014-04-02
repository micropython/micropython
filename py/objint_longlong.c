#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "mpz.h"
#include "objint.h"
#include "runtime0.h"
#include "runtime.h"

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG

// Python3 no longer has "l" suffix for long ints. We allow to use it
// for debugging purpose though.
#ifdef DEBUG
#define SUFFIX "l"
#else
#define SUFFIX ""
#endif

void int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        print(env, INT_FMT, MP_OBJ_SMALL_INT_VALUE(self_in));
    } else {
        mp_obj_int_t *self = self_in;
        print(env, "%lld" SUFFIX, self->val);
    }
}

mp_obj_t int_unary_op(int op, mp_obj_t o_in) {
    mp_obj_int_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(o->val != 0);
        case MP_UNARY_OP_POSITIVE: return o_in;
        case MP_UNARY_OP_NEGATIVE: return mp_obj_new_int_from_ll(-o->val);
        case MP_UNARY_OP_INVERT: return mp_obj_new_int_from_ll(~o->val);
        default: return NULL; // op not supported
    }
}

mp_obj_t int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    long long lhs_val;
    long long rhs_val;

    if (MP_OBJ_IS_SMALL_INT(lhs_in)) {
        lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs_in);
    } else if (MP_OBJ_IS_TYPE(lhs_in, &mp_type_int)) {
        lhs_val = ((mp_obj_int_t*)lhs_in)->val;
    } else {
        return MP_OBJ_NULL;
    }

    if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
        rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs_in);
    } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_int)) {
        rhs_val = ((mp_obj_int_t*)rhs_in)->val;
    } else {
        if (op == MP_BINARY_OP_MULTIPLY) {
            if (MP_OBJ_IS_STR(rhs_in) || MP_OBJ_IS_TYPE(rhs_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(rhs_in, &mp_type_list)) {
                // multiply is commutative for these types, so delegate to them
                return mp_binary_op(op, rhs_in, lhs_in);
            }
        }
        // unsupported operation/type
        return MP_OBJ_NULL;
    }

    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
            return mp_obj_new_int_from_ll(lhs_val + rhs_val);
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_INPLACE_SUBTRACT:
            return mp_obj_new_int_from_ll(lhs_val - rhs_val);
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY:
            return mp_obj_new_int_from_ll(lhs_val * rhs_val);
        case MP_BINARY_OP_FLOOR_DIVIDE:
        case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
            return mp_obj_new_int_from_ll(lhs_val / rhs_val);
        case MP_BINARY_OP_MODULO:
        case MP_BINARY_OP_INPLACE_MODULO:
            return mp_obj_new_int_from_ll(lhs_val % rhs_val);

        case MP_BINARY_OP_AND:
        case MP_BINARY_OP_INPLACE_AND:
            return mp_obj_new_int_from_ll(lhs_val & rhs_val);
        case MP_BINARY_OP_OR:
        case MP_BINARY_OP_INPLACE_OR:
            return mp_obj_new_int_from_ll(lhs_val | rhs_val);
        case MP_BINARY_OP_XOR:
        case MP_BINARY_OP_INPLACE_XOR:
            return mp_obj_new_int_from_ll(lhs_val ^ rhs_val);

        case MP_BINARY_OP_LSHIFT:
        case MP_BINARY_OP_INPLACE_LSHIFT:
            return mp_obj_new_int_from_ll(lhs_val << (int)rhs_val);
        case MP_BINARY_OP_RSHIFT:
        case MP_BINARY_OP_INPLACE_RSHIFT:
            return mp_obj_new_int_from_ll(lhs_val >> (int)rhs_val);

        case MP_BINARY_OP_LESS:
            return MP_BOOL(lhs_val < rhs_val);
        case MP_BINARY_OP_MORE:
            return MP_BOOL(lhs_val > rhs_val);
        case MP_BINARY_OP_LESS_EQUAL:
            return MP_BOOL(lhs_val <= rhs_val);
        case MP_BINARY_OP_MORE_EQUAL:
            return MP_BOOL(lhs_val >= rhs_val);
        case MP_BINARY_OP_EQUAL:
            return MP_BOOL(lhs_val == rhs_val);
        case MP_BINARY_OP_NOT_EQUAL:
            return MP_BOOL(lhs_val != rhs_val);

        default:
            // op not supported
            return MP_OBJ_NULL;
    }
}

mp_obj_t mp_obj_new_int(machine_int_t value) {
    if (MP_OBJ_FITS_SMALL_INT(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value) {
    // SMALL_INT accepts only signed numbers, of one bit less size
    // than word size, which totals 2 bits less for unsigned numbers.
    if ((value & (WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_ll(long long val) {
    mp_obj_int_t *o = m_new_obj(mp_obj_int_t);
    o->base.type = &mp_type_int;
    o->val = val;
    return o;
}

mp_obj_t mp_obj_new_int_from_long_str(const char *s) {
    long long v;
    char *end;
    // TODO: this doesn't handle Python hacked 0o octal syntax
    v = strtoll(s, &end, 0);
    if (*end != 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "invalid syntax for number"));
    }
    mp_obj_int_t *o = m_new_obj(mp_obj_int_t);
    o->base.type = &mp_type_int;
    o->val = v;
    return o;
}

machine_int_t mp_obj_int_get(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        return self->val;
    }
}

machine_int_t mp_obj_int_get_checked(mp_obj_t self_in) {
    // TODO: Check overflow
    return mp_obj_int_get(self_in);
}

#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_int_as_float(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        return self->val;
    }
}
#endif

#endif
