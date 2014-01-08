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
#include "builtin.h"

// args[0] is function from class body
// args[1] is class name
// args[2:] are base objects
mp_obj_t mp_builtin___build_class__(int n_args, const mp_obj_t *args) {
    assert(2 <= n_args);

    // we differ from CPython: we set the new __locals__ object here
    mp_map_t *old_locals = rt_locals_get();
    mp_map_t *class_locals = mp_map_new(0);
    rt_locals_set(class_locals);

    // call the class code
    mp_obj_t cell = rt_call_function_1(args[0], (mp_obj_t)0xdeadbeef);

    // restore old __locals__ object
    rt_locals_set(old_locals);

    /*
    // get the class type (meta object) from the base objects
    mp_obj_t meta;
    if (n_args == 2) {
        // no explicit bases, so use 'type'
        meta = (mp_obj_t)&mp_const_type;
    } else {
        // use type of first base object
        meta = mp_obj_get_type(args[2]);
    }
    */

    // TODO do proper metaclass resolution for multiple base objects

    /*
    // create the new class using a call to the meta object
    // (arguments must be backwards in the array)
    mp_obj_t meta_args[3];
    meta_args[2] = args[1]; // class name
    meta_args[1] = mp_obj_new_tuple(n_args - 2, args + 2); // tuple of bases
    meta_args[0] = class_locals; // dict of members TODO, currently is a map
    mp_obj_t new_class = rt_call_function_n(meta, 3, meta_args);
    */
    // create the new class
    mp_obj_t new_class = mp_obj_new_class(class_locals);

    // store into cell if neede
    if (cell != mp_const_none) {
        mp_obj_cell_set(cell, new_class);
    }

    return new_class;
}

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin___build_class___obj, 2, mp_builtin___build_class__);

mp_obj_t mp_builtin___repl_print__(mp_obj_t o) {
    if (o != mp_const_none) {
        mp_obj_print(o);
        printf("\n");
    }
    return mp_const_none;
}

mp_obj_t mp_builtin_abs(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(o_in);
        if (val < 0) {
            val = -val;
        }
        return MP_OBJ_NEW_SMALL_INT(val);
#if MICROPY_ENABLE_FLOAT
    } else if (MP_OBJ_IS_TYPE(o_in, &float_type)) {
        mp_float_t value = mp_obj_float_get(o_in);
        // TODO check for NaN etc
        if (value < 0) {
            return mp_obj_new_float(-value);
        } else {
            return o_in;
        }
    } else if (MP_OBJ_IS_TYPE(o_in, &complex_type)) {
        mp_float_t real, imag;
        mp_obj_complex_get(o_in, &real, &imag);
        return mp_obj_new_float(machine_sqrt(real*real + imag*imag));
#endif
    } else {
        assert(0);
        return mp_const_none;
    }
}

mp_obj_t mp_builtin_all(mp_obj_t o_in) {
    mp_obj_t iterable = rt_getiter(o_in);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        if (!rt_is_true(item)) {
            return mp_const_false;
        }
    }
    return mp_const_true;
}

mp_obj_t mp_builtin_any(mp_obj_t o_in) {
    mp_obj_t iterable = rt_getiter(o_in);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        if (rt_is_true(item)) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

mp_obj_t mp_builtin_callable(mp_obj_t o_in) {
    if (mp_obj_is_callable(o_in)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t mp_builtin_chr(mp_obj_t o_in) {
    int ord = mp_obj_get_int(o_in);
    if (0 <= ord && ord <= 0x10ffff) {
        char *str = m_new(char, 2);
        str[0] = ord;
        str[1] = '\0';
        return mp_obj_new_str(qstr_from_str_take(str, 2));
    } else {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_ValueError, "chr() arg not in range(0x110000)"));
    }
}

mp_obj_t mp_builtin_divmod(mp_obj_t o1_in, mp_obj_t o2_in) {
    if (MP_OBJ_IS_SMALL_INT(o1_in) && MP_OBJ_IS_SMALL_INT(o2_in)) {
        mp_small_int_t i1 = MP_OBJ_SMALL_INT_VALUE(o1_in);
        mp_small_int_t i2 = MP_OBJ_SMALL_INT_VALUE(o2_in);
        mp_obj_t revs_args[2];
        revs_args[1] = MP_OBJ_NEW_SMALL_INT(i1 / i2);
        revs_args[0] = MP_OBJ_NEW_SMALL_INT(i1 % i2);
        return rt_build_tuple(2, revs_args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_TypeError, "unsupported operand type(s) for divmod(): '%s' and '%s'", mp_obj_get_type_str(o1_in), mp_obj_get_type_str(o2_in)));
    }
}

static mp_obj_t mp_builtin_hash(mp_obj_t o_in) {
    // TODO hash will generally overflow small integer; can we safely truncate it?
    return mp_obj_new_int(mp_obj_hash(o_in));
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_hash_obj, mp_builtin_hash);

static mp_obj_t mp_builtin_iter(mp_obj_t o_in) {
    return rt_getiter(o_in);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_iter_obj, mp_builtin_iter);

mp_obj_t mp_builtin_len(mp_obj_t o_in) {
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
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "object of type '%s' has no len()", mp_obj_get_type_str(o_in)));
    }
    return MP_OBJ_NEW_SMALL_INT(len);
}

mp_obj_t mp_builtin_max(int n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        mp_obj_t iterable = rt_getiter(args[0]);
        mp_obj_t max_obj = NULL;
        mp_obj_t item;
        while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
            if (max_obj == NULL || mp_obj_less(max_obj, item)) {
                max_obj = item;
            }
        }
        if (max_obj == NULL) {
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_ValueError, "max() arg is an empty sequence"));
        }
        return max_obj;
    } else {
        // given many args
        mp_obj_t max_obj = args[0];
        for (int i = 1; i < n_args; i++) {
            if (mp_obj_less(max_obj, args[i])) {
                max_obj = args[i];
            }
        }
        return max_obj;
    }
}

mp_obj_t mp_builtin_min(int n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        mp_obj_t iterable = rt_getiter(args[0]);
        mp_obj_t min_obj = NULL;
        mp_obj_t item;
        while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
            if (min_obj == NULL || mp_obj_less(item, min_obj)) {
                min_obj = item;
            }
        }
        if (min_obj == NULL) {
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_ValueError, "min() arg is an empty sequence"));
        }
        return min_obj;
    } else {
        // given many args
        mp_obj_t min_obj = args[0];
        for (int i = 1; i < n_args; i++) {
            if (mp_obj_less(args[i], min_obj)) {
                min_obj = args[i];
            }
        }
        return min_obj;
    }
}

static mp_obj_t mp_builtin_next(mp_obj_t o) {
    mp_obj_t ret = rt_iternext(o);
    if (ret == mp_const_stop_iteration) {
        nlr_jump(mp_obj_new_exception(MP_QSTR_StopIteration));
    } else {
        return ret;
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_next_obj, mp_builtin_next);

mp_obj_t mp_builtin_ord(mp_obj_t o_in) {
    const char *str = qstr_str(mp_obj_get_qstr(o_in));
    if (strlen(str) == 1) {
        return mp_obj_new_int(str[0]);
    } else {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "ord() expected a character, but string of length %d found", (void*)(machine_int_t)strlen(str)));
    }
}

mp_obj_t mp_builtin_pow(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 2: return rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]);
        case 3: return rt_binary_op(RT_BINARY_OP_MODULO, rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]), args[2]); // TODO optimise...
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "pow expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args));
    }
}

mp_obj_t mp_builtin_print(int n_args, const mp_obj_t *args) {
    for (int i = 0; i < n_args; i++) {
        if (i > 0) {
            printf(" ");
        }
        if (MP_OBJ_IS_TYPE(args[i], &str_type)) {
            // special case, print string raw
            printf("%s", qstr_str(mp_obj_str_get(args[i])));
        } else {
            // print the object Python style
            mp_obj_print(args[i]);
        }
    }
    printf("\n");
    return mp_const_none;
}

mp_obj_t mp_builtin_range(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 1: return mp_obj_new_range(0, mp_obj_get_int(args[0]), 1);
        case 2: return mp_obj_new_range(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), 1);
        case 3: return mp_obj_new_range(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "range expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args));
    }
}

mp_obj_t mp_builtin_sum(int n_args, const mp_obj_t *args) {
    mp_obj_t value;
    switch (n_args) {
        case 1: value = mp_obj_new_int(0); break;
        case 2: value = args[1]; break;
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "sum expected at most 2 arguments, got %d", (void*)(machine_int_t)n_args));
    }
    mp_obj_t iterable = rt_getiter(args[0]);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        value = rt_binary_op(RT_BINARY_OP_ADD, value, item);
    }
    return value;
}

static mp_obj_t mp_builtin_type(mp_obj_t o_in) {
    // TODO implement the 3 argument version of type()
    return mp_obj_get_type(o_in);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_type_obj, mp_builtin_type);
