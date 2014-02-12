#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "objint.h"
#include "runtime0.h"

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG

STATIC mp_obj_t mp_obj_new_int_from_ll(long long val);

// Python3 no longer has "l" suffix for long ints. We allow to use it
// for debugging purpose though.
#ifdef DEBUG
#define SUFFIX "l"
#else
#define SUFFIX ""
#endif

void int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        print(env, "%d", (int)MP_OBJ_SMALL_INT_VALUE(self_in));
    } else {
        mp_obj_int_t *self = self_in;
        print(env, "%lld" SUFFIX, self->val);
    }
}

mp_obj_t int_unary_op(int op, mp_obj_t o_in) {
    mp_obj_int_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_BOOL: return MP_BOOL(o->val != 0);
        case RT_UNARY_OP_POSITIVE: return o_in;
        case RT_UNARY_OP_NEGATIVE: return mp_obj_new_int_from_ll(-o->val);
        case RT_UNARY_OP_INVERT: return mp_obj_new_int_from_ll(~o->val);
        default: return NULL; // op not supported
    }
}

mp_obj_t int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_int_t *lhs = lhs_in;
    mp_obj_int_t *rhs = rhs_in;
    long long rhs_val;

    if (MP_OBJ_IS_SMALL_INT(rhs)) {
        rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs);
    } else if (MP_OBJ_IS_TYPE(rhs, &int_type)) {
        rhs_val = rhs->val;
    } else {
        return MP_OBJ_NULL;
    }

    switch (op) {
        case RT_BINARY_OP_ADD:
            return mp_obj_new_int_from_ll(lhs->val + rhs_val);
        case RT_BINARY_OP_SUBTRACT:
            return mp_obj_new_int_from_ll(lhs->val - rhs_val);
        case RT_BINARY_OP_MULTIPLY:
            return mp_obj_new_int_from_ll(lhs->val * rhs_val);
        case RT_BINARY_OP_FLOOR_DIVIDE:
            return mp_obj_new_int_from_ll(lhs->val / rhs_val);
        case RT_BINARY_OP_MODULO:
            return mp_obj_new_int_from_ll(lhs->val % rhs_val);

        case RT_BINARY_OP_INPLACE_ADD:
            lhs->val += rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_SUBTRACT:
            lhs->val -= rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_MULTIPLY:
            lhs->val *= rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_FLOOR_DIVIDE:
            lhs->val /= rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_MODULO:
            lhs->val %= rhs_val; return lhs;

        case RT_BINARY_OP_AND:
            return mp_obj_new_int_from_ll(lhs->val & rhs_val);
        case RT_BINARY_OP_OR:
            return mp_obj_new_int_from_ll(lhs->val | rhs_val);
        case RT_BINARY_OP_XOR:
            return mp_obj_new_int_from_ll(lhs->val ^ rhs_val);

        case RT_BINARY_OP_INPLACE_AND:
            lhs->val &= rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_OR:
            lhs->val |= rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_XOR:
            lhs->val ^= rhs_val; return lhs;

        case RT_BINARY_OP_LSHIFT:
            return mp_obj_new_int_from_ll(lhs->val << (int)rhs_val);
        case RT_BINARY_OP_RSHIFT:
            return mp_obj_new_int_from_ll(lhs->val >> (int)rhs_val);

        case RT_BINARY_OP_INPLACE_LSHIFT:
            lhs->val <<= (int)rhs_val; return lhs;
        case RT_BINARY_OP_INPLACE_RSHIFT:
            lhs->val >>= (int)rhs_val; return lhs;

        case RT_BINARY_OP_LESS:
            return MP_BOOL(lhs->val < rhs_val);
        case RT_BINARY_OP_MORE:
            return MP_BOOL(lhs->val > rhs_val);
        case RT_BINARY_OP_LESS_EQUAL:
            return MP_BOOL(lhs->val <= rhs_val);
        case RT_BINARY_OP_MORE_EQUAL:
            return MP_BOOL(lhs->val >= rhs_val);
        case RT_BINARY_OP_EQUAL:
            return MP_BOOL(lhs->val == rhs_val);
        case RT_BINARY_OP_NOT_EQUAL:
            return MP_BOOL(lhs->val != rhs_val);

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
    o->base.type = &int_type;
    o->val = val;
    return o;
}

mp_obj_t mp_obj_new_int_from_long_str(const char *s) {
    long long v;
    char *end;
    // TODO: this doesn't handle Python hacked 0o octal syntax
    v = strtoll(s, &end, 0);
    assert(*end == 0);
    mp_obj_int_t *o = m_new_obj(mp_obj_int_t);
    o->base.type = &int_type;
    o->val = v;
    return o;
}

machine_int_t mp_obj_int_get(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    }
    mp_obj_int_t *self = self_in;
    return self->val;
}

machine_int_t mp_obj_int_get_checked(mp_obj_t self_in) {
    // TODO: Check overflow
    return mp_obj_int_get(self_in);
}

#endif
