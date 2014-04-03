#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "parsenumbase.h"
#include "obj.h"
#include "mpz.h"
#include "objint.h"
#include "runtime0.h"
#include "runtime.h"

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ

STATIC mp_obj_int_t *mp_obj_int_new_mpz(void) {
    mp_obj_int_t *o = m_new_obj(mp_obj_int_t);
    o->base.type = &mp_type_int;
    mpz_init_zero(&o->mpz);
    return o;
}

void int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        print(env, INT_FMT, MP_OBJ_SMALL_INT_VALUE(self_in));
    } else {
        // TODO would rather not allocate memory to print...
        mp_obj_int_t *self = self_in;
        char *str = mpz_as_str(&self->mpz, 10);
        print(env, "%s", str);
        m_free(str, 0);
    }
}

mp_obj_t int_unary_op(int op, mp_obj_t o_in) {
    mp_obj_int_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(!mpz_is_zero(&o->mpz));
        case MP_UNARY_OP_POSITIVE: return o_in;
        case MP_UNARY_OP_NEGATIVE: { mp_obj_int_t *o2 = mp_obj_int_new_mpz(); mpz_neg_inpl(&o2->mpz, &o->mpz); return o2; }
        case MP_UNARY_OP_INVERT: { mp_obj_int_t *o2 = mp_obj_int_new_mpz(); mpz_not_inpl(&o2->mpz, &o->mpz); return o2; }
        default: return NULL; // op not supported
    }
}

mp_obj_t int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    const mpz_t *zlhs;
    const mpz_t *zrhs;
    mpz_t z_int;
    mpz_dig_t z_int_dig[MPZ_NUM_DIG_FOR_INT];

    // lhs could be a small int (eg small-int + mpz)
    if (MP_OBJ_IS_SMALL_INT(lhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, MPZ_NUM_DIG_FOR_INT, MP_OBJ_SMALL_INT_VALUE(lhs_in));
        zlhs = &z_int;
    } else if (MP_OBJ_IS_TYPE(lhs_in, &mp_type_int)) {
        zlhs = &((mp_obj_int_t*)lhs_in)->mpz;
    } else {
        // unsupported type
        return MP_OBJ_NULL;
    }

    // if rhs is small int, then lhs was not (otherwise mp_binary_op handles it)
    if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, MPZ_NUM_DIG_FOR_INT, MP_OBJ_SMALL_INT_VALUE(rhs_in));
        zrhs = &z_int;
    } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_int)) {
        zrhs = &((mp_obj_int_t*)rhs_in)->mpz;
#if MICROPY_ENABLE_FLOAT
    } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_float)) {
        return mp_obj_float_binary_op(op, mpz_as_float(zlhs), rhs_in);
    } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_complex)) {
        return mp_obj_complex_binary_op(op, mpz_as_float(zlhs), 0, rhs_in);
#endif
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

    if (0) {
#if MICROPY_ENABLE_FLOAT
    } else if (op == MP_BINARY_OP_TRUE_DIVIDE || op == MP_BINARY_OP_INPLACE_TRUE_DIVIDE) {
        mp_float_t flhs = mpz_as_float(zlhs);
        mp_float_t frhs = mpz_as_float(zrhs);
        return mp_obj_new_float(flhs / frhs);
#endif

    } else if (op <= MP_BINARY_OP_INPLACE_POWER) {
        mp_obj_int_t *res = mp_obj_int_new_mpz();

        switch (op) {
            case MP_BINARY_OP_ADD:
            case MP_BINARY_OP_INPLACE_ADD:
                mpz_add_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_SUBTRACT:
            case MP_BINARY_OP_INPLACE_SUBTRACT:
                mpz_sub_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_MULTIPLY:
            case MP_BINARY_OP_INPLACE_MULTIPLY:
                mpz_mul_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_FLOOR_DIVIDE:
            case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE: {
                mpz_t rem; mpz_init_zero(&rem);
                mpz_divmod_inpl(&res->mpz, &rem, zlhs, zrhs);
		        if (zlhs->neg != zrhs->neg) {
                    if (!mpz_is_zero(&rem)) {
                        mpz_t mpzone; mpz_init_from_int(&mpzone, -1);
                        mpz_add_inpl(&res->mpz, &res->mpz, &mpzone);
                    }
                }
                mpz_deinit(&rem);
                break;
            }
            case MP_BINARY_OP_MODULO:
            case MP_BINARY_OP_INPLACE_MODULO: {
                mpz_t quo; mpz_init_zero(&quo);
                mpz_divmod_inpl(&quo, &res->mpz, zlhs, zrhs);
                mpz_deinit(&quo);
		        // Check signs and do Python style modulo
		        if (zlhs->neg != zrhs->neg) {
                    mpz_add_inpl(&res->mpz, &res->mpz, zrhs);
                }
                break;
            }

            case MP_BINARY_OP_AND:
            case MP_BINARY_OP_INPLACE_AND:
                mpz_and_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_OR:
            case MP_BINARY_OP_INPLACE_OR:
                mpz_or_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_XOR:
            case MP_BINARY_OP_INPLACE_XOR:
                mpz_xor_inpl(&res->mpz, zlhs, zrhs);
                break;

            case MP_BINARY_OP_LSHIFT:
            case MP_BINARY_OP_INPLACE_LSHIFT:
            case MP_BINARY_OP_RSHIFT:
            case MP_BINARY_OP_INPLACE_RSHIFT: {
                // TODO check conversion overflow
                machine_int_t irhs = mpz_as_int(zrhs);
                if (irhs < 0) {
                    nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "negative shift count"));
                }
                if (op == MP_BINARY_OP_LSHIFT || op == MP_BINARY_OP_INPLACE_LSHIFT) {
                    mpz_shl_inpl(&res->mpz, zlhs, irhs);
                } else {
                    mpz_shr_inpl(&res->mpz, zlhs, irhs);
                }
                break;
            }

            case MP_BINARY_OP_POWER:
            case MP_BINARY_OP_INPLACE_POWER:
                mpz_pow_inpl(&res->mpz, zlhs, zrhs);
                break;

            default:
                return MP_OBJ_NULL;
        }

        return res;

    } else {
        int cmp = mpz_cmp(zlhs, zrhs);
        switch (op) {
            case MP_BINARY_OP_LESS:
                return MP_BOOL(cmp < 0);
            case MP_BINARY_OP_MORE:
                return MP_BOOL(cmp > 0);
            case MP_BINARY_OP_LESS_EQUAL:
                return MP_BOOL(cmp <= 0);
            case MP_BINARY_OP_MORE_EQUAL:
                return MP_BOOL(cmp >= 0);
            case MP_BINARY_OP_EQUAL:
                return MP_BOOL(cmp == 0);
            case MP_BINARY_OP_NOT_EQUAL:
                return MP_BOOL(cmp != 0);

            default:
                return MP_OBJ_NULL;
        }
    }
}

mp_obj_t mp_obj_new_int(machine_int_t value) {
    if (MP_OBJ_FITS_SMALL_INT(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_ll(long long val) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    mpz_set_from_ll(&o->mpz, val);
    return o;
}

mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value) {
    // SMALL_INT accepts only signed numbers, of one bit less size
    // than word size, which totals 2 bits less for unsigned numbers.
    if ((value & (WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_long_str(const char *str) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    uint len = strlen(str);
    int base = 0;
    int skip = mp_parse_num_base(str, len, &base);
    str += skip;
    len -= skip;
    uint n = mpz_set_from_str(&o->mpz, str, len, false, base);
    if (n != len) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "invalid syntax for number"));
    }
    return o;
}

machine_int_t mp_obj_int_get(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        return mpz_as_int(&self->mpz);
    }
}

machine_int_t mp_obj_int_get_checked(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        machine_int_t value;
        if (mpz_as_int_checked(&self->mpz, &value)) {
            return value;
        } else {
            // overflow
            nlr_jump(mp_obj_new_exception_msg(&mp_type_OverflowError, "overflow converting long int to machine word"));
        }
    }
}

#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_int_as_float(mp_obj_t self_in) {
    if (MP_OBJ_IS_SMALL_INT(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        return mpz_as_float(&self->mpz);
    }
}
#endif

#endif
