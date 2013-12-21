#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
//#include "bc.h"
#include "map.h"
#include "builtin.h"

mp_obj_t mp_builtin___build_class__(mp_obj_t o_class_fun, mp_obj_t o_class_name) {
    // we differ from CPython: we set the new __locals__ object here
    mp_map_t *old_locals = rt_get_map_locals();
    mp_map_t *class_locals = mp_map_new(MP_MAP_QSTR, 0);
    rt_set_map_locals(class_locals);

    // call the class code
    rt_call_function_1(o_class_fun, (mp_obj_t)0xdeadbeef);

    // restore old __locals__ object
    rt_set_map_locals(old_locals);

    // create and return the new class
    return mp_obj_new_class(class_locals);
}

mp_obj_t mp_builtin___import__(int n, mp_obj_t *args) {
    printf("import:\n");
    for (int i = 0; i < n; i++) {
    printf("  ");
    mp_obj_print(args[i]);
    printf("\n");
    }
    return mp_const_none;
}

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

mp_obj_t mp_builtin_bool(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0: return mp_const_false;
        case 1: if (rt_is_true(args[0])) { return mp_const_true; } else { return mp_const_false; }
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "bool() takes at most 1 argument (%d given)", (void*)(machine_int_t)n_args));
    }
}

mp_obj_t mp_builtin_callable(mp_obj_t o_in) {
    if (mp_obj_is_callable(o_in)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

#if MICROPY_ENABLE_FLOAT
mp_obj_t mp_builtin_complex(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0:
            return mp_obj_new_complex(0, 0);

        case 1:
            // TODO allow string as first arg
            if (MP_OBJ_IS_TYPE(args[0], &complex_type)) {
                return args[0];
            } else {
                return mp_obj_new_complex(mp_obj_get_float(args[0]), 0);
            }

        case 2:
        {
            mp_float_t real, imag;
            if (MP_OBJ_IS_TYPE(args[0], &complex_type)) {
                mp_obj_get_complex(args[0], &real, &imag);
            } else {
                real = mp_obj_get_float(args[0]);
                imag = 0;
            }
            if (MP_OBJ_IS_TYPE(args[1], &complex_type)) {
                mp_float_t real2, imag2;
                mp_obj_get_complex(args[1], &real2, &imag2);
                real -= imag2;
                imag += real2;
            } else {
                imag += mp_obj_get_float(args[1]);
            }
            return mp_obj_new_complex(real, imag);
        }

        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "comlpex() takes at most 2 arguments (%d given)", (void*)(machine_int_t)n_args));
    }
}
#endif

mp_obj_t mp_builtin_chr(mp_obj_t o_in) {
    int ord = mp_obj_get_int(o_in);
    if (0 <= ord && ord <= 0x10ffff) {
        char *str = m_new(char, 2);
        str[0] = ord;
        str[1] = '\0';
        return mp_obj_new_str(qstr_from_str_take(str));
    } else {
        nlr_jump(mp_obj_new_exception_msg(rt_q_ValueError, "chr() arg not in range(0x110000)"));
    }
}

mp_obj_t mp_builtin_dict(void) {
    // TODO create from an iterable!
    return rt_build_map(0);
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
        nlr_jump(mp_obj_new_exception_msg_2_args(rt_q_TypeError, "unsupported operand type(s) for divmod(): '%s' and '%s'", mp_obj_get_type_str(o1_in), mp_obj_get_type_str(o2_in)));
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
        /* TODO
    } else if (MP_OBJ_IS_TYPE(o_in, &dict_type)) {
        mp_obj_base_t *o = o_in;
        len = o->u_map.used;
        */
    } else {
        nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "object of type '%s' has no len()", mp_obj_get_type_str(o_in)));
    }
    return MP_OBJ_NEW_SMALL_INT(len);
}

mp_obj_t mp_builtin_list(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0: return rt_build_list(0, NULL);
        case 1:
        {
            // make list from iterable
            mp_obj_t iterable = rt_getiter(args[0]);
            mp_obj_t list = rt_build_list(0, NULL);
            mp_obj_t item;
            while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
                rt_list_append(list, item);
            }
            return list;
        }
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "list() takes at most 1 argument (%d given)", (void*)(machine_int_t)n_args));
    }
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
            nlr_jump(mp_obj_new_exception_msg(rt_q_ValueError, "max() arg is an empty sequence"));
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
            nlr_jump(mp_obj_new_exception_msg(rt_q_ValueError, "min() arg is an empty sequence"));
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

static mp_obj_t mp_builtin_next(mp_obj_t o_in) {
    return mp_obj_gen_instance_next(o_in);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_next_obj, mp_builtin_next);

mp_obj_t mp_builtin_ord(mp_obj_t o_in) {
    const char *str = qstr_str(mp_obj_get_qstr(o_in));
    if (strlen(str) == 1) {
        return mp_obj_new_int(str[0]);
    } else {
        nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "ord() expected a character, but string of length %d found", (void*)(machine_int_t)strlen(str)));
    }
}

mp_obj_t mp_builtin_pow(int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 2: return rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]);
        case 3: return rt_binary_op(RT_BINARY_OP_MODULO, rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]), args[2]); // TODO optimise...
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "pow expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args));
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
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "range expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args));
    }
}

static mp_obj_t mp_builtin_set(int n_args, const mp_obj_t *args) {
    assert(0 <= n_args && n_args <= 1);

    if (n_args == 0) {
        // return a new, empty set
        return mp_obj_new_set(0, NULL);
    } else {
        // 1 argument, an iterable from which we make a new set
        mp_obj_t set = mp_obj_new_set(0, NULL);
        mp_obj_t iterable = rt_getiter(args[0]);
        mp_obj_t item;
        while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
            mp_obj_set_store(set, item);
        }
        return set;
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_set_obj, 0, 1, mp_builtin_set);

mp_obj_t mp_builtin_sum(int n_args, const mp_obj_t *args) {
    mp_obj_t value;
    switch (n_args) {
        case 1: value = mp_obj_new_int(0); break;
        case 2: value = args[1]; break;
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(rt_q_TypeError, "sum expected at most 2 arguments, got %d", (void*)(machine_int_t)n_args));
    }
    mp_obj_t iterable = rt_getiter(args[0]);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        value = rt_binary_op(RT_BINARY_OP_ADD, value, item);
    }
    return value;
}
