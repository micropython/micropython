#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"

mp_obj_t mp_obj_get_type(mp_obj_t o_in) {
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
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return "int";
    } else if (MP_OBJ_IS_QSTR(o_in)) {
        return "str";
    } else {
        mp_obj_base_t *o = o_in;
        return o->type->name;
    }
}

void printf_wrapper(void *env, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void mp_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        print(env, "%d", (int)MP_OBJ_SMALL_INT_VALUE(o_in));
    } else if (MP_OBJ_IS_QSTR(o_in)) {
        mp_obj_str_print_qstr(print, env, MP_OBJ_QSTR_VALUE(o_in), kind);
    } else {
        mp_obj_base_t *o = o_in;
        if (o->type->print != NULL) {
            o->type->print(print, env, o_in, kind);
        } else {
            print(env, "<%s>", o->type->name);
        }
    }
}

void mp_obj_print(mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_print_helper(printf_wrapper, NULL, o_in, kind);
}

// helper function to print an exception with traceback
void mp_obj_print_exception(mp_obj_t exc) {
    if (MP_OBJ_IS_TYPE(exc, &exception_type)) {
        machine_uint_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            printf("Traceback (most recent call last):\n");
            for (int i = n - 3; i >= 0; i -= 3) {
                printf("  File \"%s\", line %d, in %s\n", qstr_str(values[i]), (int)values[i + 1], qstr_str(values[i + 2]));
            }
        }
    }
    mp_obj_print(exc, PRINT_REPR);
    printf("\n");
}

bool mp_obj_is_callable(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return false;
    } else {
        mp_obj_base_t *o = o_in;
        return o->type->call != NULL;
    }
}

machine_int_t mp_obj_hash(mp_obj_t o_in) {
    if (o_in == mp_const_false) {
        return 0; // needs to hash to same as the integer 0, since False==0
    } else if (o_in == mp_const_true) {
        return 1; // needs to hash to same as the integer 1, since True==1
    } else if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return MP_OBJ_SMALL_INT_VALUE(o_in);
    } else if (MP_OBJ_IS_QSTR(o_in)) {
        return MP_OBJ_QSTR_VALUE(o_in);
    } else if (MP_OBJ_IS_TYPE(o_in, &none_type)) {
        return (machine_int_t)o_in;
    } else if (MP_OBJ_IS_TYPE(o_in, &str_type)) {
        return mp_obj_str_get(o_in);
    } else {
        assert(0);
        return 0;
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
                    mp_obj_t r = o->type->binary_op(RT_COMPARE_OP_EQUAL, o2, o1);
                    return r == mp_const_true ? true : false;
                }
            }
            return false;
        }
    } else if (MP_OBJ_IS_QSTR(o1) || MP_OBJ_IS_QSTR(o2)) {
        return false;
    } else if (MP_OBJ_IS_TYPE(o1, &str_type) && MP_OBJ_IS_TYPE(o2, &str_type)) {
        return mp_obj_str_get(o1) == mp_obj_str_get(o2);
    } else {
        mp_obj_base_t *o = o1;
        if (o->type->binary_op != NULL) {
            mp_obj_t r = o->type->binary_op(RT_COMPARE_OP_EQUAL, o1, o2);
            if (r != MP_OBJ_NULL) {
                return r == mp_const_true ? true : false;
            }
        }
        // TODO: Debugging helper
        printf("Equality for '%s' and '%s' types not yet implemented\n", mp_obj_get_type_str(o1), mp_obj_get_type_str(o2));
        assert(0);
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
#if MICROPY_ENABLE_FLOAT
    } else if (MP_OBJ_IS_TYPE(arg, &float_type)) {
        // TODO work out if this should be floor, ceil or trunc
        return (machine_int_t)mp_obj_float_get(arg);
#endif
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "can't convert %s to int", mp_obj_get_type_str(arg)));
    }
}

#if MICROPY_ENABLE_FLOAT
machine_float_t mp_obj_get_float(mp_obj_t arg) {
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        return MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &float_type)) {
        return mp_obj_float_get(arg);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "can't convert %s to float", mp_obj_get_type_str(arg)));
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
    } else if (MP_OBJ_IS_TYPE(arg, &float_type)) {
        *real = mp_obj_float_get(arg);
        *imag = 0;
    } else if (MP_OBJ_IS_TYPE(arg, &complex_type)) {
        mp_obj_complex_get(arg, real, imag);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "can't convert %s to complex", mp_obj_get_type_str(arg)));
    }
}
#endif

qstr mp_obj_get_qstr(mp_obj_t arg) {
    if (MP_OBJ_IS_QSTR(arg)) {
        return MP_OBJ_QSTR_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &str_type)) {
        return mp_obj_str_get(arg);
    } else {
        assert(0);
        return 0;
    }
}

mp_obj_t *mp_obj_get_array_fixed_n(mp_obj_t o_in, machine_int_t n) {
    if (MP_OBJ_IS_TYPE(o_in, &tuple_type) || MP_OBJ_IS_TYPE(o_in, &list_type)) {
        uint seq_len;
        mp_obj_t *seq_items;
        if (MP_OBJ_IS_TYPE(o_in, &tuple_type)) {
            mp_obj_tuple_get(o_in, &seq_len, &seq_items);
        } else {
            mp_obj_list_get(o_in, &seq_len, &seq_items);
        }
        if (seq_len != n) {
            nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_IndexError, "requested length %d but object has length %d", n, seq_len));
        }
        return seq_items;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "object '%s' is not a tuple or list", mp_obj_get_type_str(o_in)));
    }
}

uint mp_get_index(const mp_obj_type_t *type, machine_uint_t len, mp_obj_t index) {
    // TODO False and True are considered 0 and 1 for indexing purposes
    if (MP_OBJ_IS_SMALL_INT(index)) {
        int i = MP_OBJ_SMALL_INT_VALUE(index);
        if (i < 0) {
            i += len;
        }
        if (i < 0 || i >= len) {
            nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_IndexError, "%s index out of range", type->name));
        }
        return i;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "%s indices must be integers, not %s", type->name, mp_obj_get_type_str(index)));
    }
}

// may return MP_OBJ_NULL
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in) {
    mp_small_int_t len = 0;
    if (MP_OBJ_IS_TYPE(o_in, &str_type)) {
        len = strlen(qstr_str(mp_obj_str_get(o_in)));
    } else if (MP_OBJ_IS_TYPE(o_in, &tuple_type)) {
        uint seq_len;
        mp_obj_t *seq_items;
        mp_obj_tuple_get(o_in, &seq_len, &seq_items);
        len = seq_len;
    } else if (MP_OBJ_IS_TYPE(o_in, &list_type)) {
        uint seq_len;
        mp_obj_t *seq_items;
        mp_obj_list_get(o_in, &seq_len, &seq_items);
        len = seq_len;
    } else if (MP_OBJ_IS_TYPE(o_in, &dict_type)) {
        len = mp_obj_dict_len(o_in);
    } else {
        return MP_OBJ_NULL;
    }
    return MP_OBJ_NEW_SMALL_INT(len);
}

// Return input argument. Useful as .getiter for objects which are
// their own iterators, etc.
mp_obj_t mp_identity(mp_obj_t self) {
    return self;
}
