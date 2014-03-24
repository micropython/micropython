#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"

mp_obj_type_t *mp_obj_get_type(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return (mp_obj_t)&int_type;
    } else if (MP_OBJ_IS_QSTR(o_in)) {
        return (mp_obj_t)&str_type;
    } else {
        mp_obj_base_t *o = o_in;
        return (mp_obj_t)o->type;
    }
}

const char *mp_obj_get_type_str(mp_obj_t o_in) {
    return qstr_str(mp_obj_get_type(o_in)->name);
}

void printf_wrapper(void *env, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void mp_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->print != NULL) {
        type->print(print, env, o_in, kind);
    } else {
        print(env, "<%s>", qstr_str(type->name));
    }
}

void mp_obj_print(mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_print_helper(printf_wrapper, NULL, o_in, kind);
}

// helper function to print an exception with traceback
void mp_obj_print_exception(mp_obj_t exc) {
    if (mp_obj_is_exception_instance(exc)) {
        machine_uint_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            printf("Traceback (most recent call last):\n");
            for (int i = n - 3; i >= 0; i -= 3) {
#if MICROPY_ENABLE_SOURCE_LINE
                printf("  File \"%s\", line %d, in %s\n", qstr_str(values[i]), (int)values[i + 1], qstr_str(values[i + 2]));
#else
                printf("  File \"%s\", in %s\n", qstr_str(values[i]), qstr_str(values[i + 2]));
#endif
            }
        }
    }
    mp_obj_print(exc, PRINT_REPR);
    printf("\n");
}

bool mp_obj_is_callable(mp_obj_t o_in) {
    return mp_obj_get_type(o_in)->call != NULL;
}

machine_int_t mp_obj_hash(mp_obj_t o_in) {
    if (o_in == mp_const_false) {
        return 0; // needs to hash to same as the integer 0, since False==0
    } else if (o_in == mp_const_true) {
        return 1; // needs to hash to same as the integer 1, since True==1
    } else if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return MP_OBJ_SMALL_INT_VALUE(o_in);
    } else if (MP_OBJ_IS_STR(o_in)) {
        return mp_obj_str_get_hash(o_in);
    } else if (MP_OBJ_IS_TYPE(o_in, &none_type)) {
        return (machine_int_t)o_in;
    } else if (MP_OBJ_IS_TYPE(o_in, &fun_native_type) || MP_OBJ_IS_TYPE(o_in, &fun_bc_type)) {
        return (machine_int_t)o_in;
    } else if (MP_OBJ_IS_TYPE(o_in, &tuple_type)) {
        return mp_obj_tuple_hash(o_in);

    // TODO hash class and instances
    // TODO delegate to __hash__ method if it exists

    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "unhashable type: '%s'", mp_obj_get_type_str(o_in)));
    }
}

// this function implements the '==' operator (and so the inverse of '!=')
// from the python language reference:
// "The objects need not have the same type. If both are numbers, they are converted
// to a common type. Otherwise, the == and != operators always consider objects of
// different types to be unequal."
// note also that False==0 and True==1 are true expressions
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2) {
    if (o1 == o2) {
        return true;
    } else if (MP_OBJ_IS_SMALL_INT(o1) || MP_OBJ_IS_SMALL_INT(o2)) {
        if (MP_OBJ_IS_SMALL_INT(o1) && MP_OBJ_IS_SMALL_INT(o2)) {
            return false;
        } else {
            if (MP_OBJ_IS_SMALL_INT(o2)) {
                mp_obj_t temp = o1; o1 = o2; o2 = temp;
            }
            // o1 is the SMALL_INT, o2 is not
            mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(o1);
            if (o2 == mp_const_false) {
                return val == 0;
            } else if (o2 == mp_const_true) {
                return val == 1;
            } else if (MP_OBJ_IS_TYPE(o2, &int_type)) {
                // If o2 is long int, dispatch to its virtual methods
                mp_obj_base_t *o = o2;
                if (o->type->binary_op != NULL) {
                    mp_obj_t r = o->type->binary_op(RT_BINARY_OP_EQUAL, o2, o1);
                    return r == mp_const_true ? true : false;
                }
            }
            return false;
        }
    } else if (MP_OBJ_IS_STR(o1) && MP_OBJ_IS_STR(o2)) {
        return mp_obj_str_equal(o1, o2);
    } else {
        mp_obj_base_t *o = o1;
        if (o->type->binary_op != NULL) {
            mp_obj_t r = o->type->binary_op(RT_BINARY_OP_EQUAL, o1, o2);
            if (r != MP_OBJ_NULL) {
                return r == mp_const_true ? true : false;
            }
        }

        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_NotImplementedError,
            "Equality for '%s' and '%s' types not yet implemented", mp_obj_get_type_str(o1), mp_obj_get_type_str(o2)));
        return false;
    }
}

bool mp_obj_less(mp_obj_t o1, mp_obj_t o2) {
    if (MP_OBJ_IS_SMALL_INT(o1) && MP_OBJ_IS_SMALL_INT(o2)) {
        mp_small_int_t i1 = MP_OBJ_SMALL_INT_VALUE(o1);
        mp_small_int_t i2 = MP_OBJ_SMALL_INT_VALUE(o2);
        return i1 < i2;
    } else {
        assert(0);
        return false;
    }
}

machine_int_t mp_obj_get_int(mp_obj_t arg) {
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        return MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &int_type)) {
        return mp_obj_int_get_checked(arg);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "can't convert %s to int", mp_obj_get_type_str(arg)));
    }
}

#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_get_float(mp_obj_t arg) {
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        return MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &int_type)) {
        return mp_obj_int_as_float(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_float)) {
        return mp_obj_float_get(arg);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "can't convert %s to float", mp_obj_get_type_str(arg)));
    }
}

void mp_obj_get_complex(mp_obj_t arg, mp_float_t *real, mp_float_t *imag) {
    if (arg == mp_const_false) {
        *real = 0;
        *imag = 0;
    } else if (arg == mp_const_true) {
        *real = 1;
        *imag = 0;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        *real = MP_OBJ_SMALL_INT_VALUE(arg);
        *imag = 0;
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_float)) {
        *real = mp_obj_float_get(arg);
        *imag = 0;
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_complex)) {
        mp_obj_complex_get(arg, real, imag);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "can't convert %s to complex", mp_obj_get_type_str(arg)));
    }
}
#endif

void mp_obj_get_array(mp_obj_t o, uint *len, mp_obj_t **items) {
    if (MP_OBJ_IS_TYPE(o, &tuple_type)) {
        mp_obj_tuple_get(o, len, items);
    } else if (MP_OBJ_IS_TYPE(o, &list_type)) {
        mp_obj_list_get(o, len, items);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "object '%s' is not a tuple or list", mp_obj_get_type_str(o)));
    }
}

void mp_obj_get_array_fixed_n(mp_obj_t o, uint len, mp_obj_t **items) {
    if (MP_OBJ_IS_TYPE(o, &tuple_type) || MP_OBJ_IS_TYPE(o, &list_type)) {
        uint seq_len;
        if (MP_OBJ_IS_TYPE(o, &tuple_type)) {
            mp_obj_tuple_get(o, &seq_len, items);
        } else {
            mp_obj_list_get(o, &seq_len, items);
        }
        if (seq_len != len) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "requested length %d but object has length %d", len, seq_len));
        }
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "object '%s' is not a tuple or list", mp_obj_get_type_str(o)));
    }
}

// is_slice determines whether the index is a slice index
uint mp_get_index(const mp_obj_type_t *type, machine_uint_t len, mp_obj_t index, bool is_slice) {
    int i;
    if (MP_OBJ_IS_SMALL_INT(index)) {
        i = MP_OBJ_SMALL_INT_VALUE(index);
    } else if (MP_OBJ_IS_TYPE(index, &bool_type)) {
        i = (index == mp_const_true ? 1 : 0);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "%s indices must be integers, not %s", qstr_str(type->name), mp_obj_get_type_str(index)));
    }

    if (i < 0) {
        i += len;
    }
    if (is_slice) {
        if (i < 0) {
            i = 0;
        } else if (i > len) {
            i = len;
        }
    } else {
        if (i < 0 || i >= len) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "%s index out of range", qstr_str(type->name)));
        }
    }
    return i;
}

// may return MP_OBJ_NULL
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in) {
    if (MP_OBJ_IS_STR(o_in)) {
        return MP_OBJ_NEW_SMALL_INT((machine_int_t)mp_obj_str_get_len(o_in));
    } else {
        mp_obj_type_t *type = mp_obj_get_type(o_in);
        if (type->unary_op != NULL) {
            return type->unary_op(RT_UNARY_OP_LEN, o_in);
        } else {
            return MP_OBJ_NULL;
        }
    }
}

// Return input argument. Useful as .getiter for objects which are
// their own iterators, etc.
mp_obj_t mp_identity(mp_obj_t self) {
    return self;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_identity_obj, mp_identity);
