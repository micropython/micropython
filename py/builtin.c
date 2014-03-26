#include <stdio.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"

#if MICROPY_ENABLE_FLOAT
#include <math.h>
#endif

// args[0] is function from class body
// args[1] is class name
// args[2:] are base objects
STATIC mp_obj_t mp_builtin___build_class__(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args);

    // we differ from CPython: we set the new __locals__ object here
    mp_map_t *old_locals = rt_locals_get();
    mp_obj_t class_locals = mp_obj_new_dict(0);
    rt_locals_set(mp_obj_dict_get_map(class_locals));

    // call the class code
    mp_obj_t cell = rt_call_function_1(args[0], (mp_obj_t)0xdeadbeef);

    // restore old __locals__ object
    rt_locals_set(old_locals);

    // get the class type (meta object) from the base objects
    mp_obj_t meta;
    if (n_args == 2) {
        // no explicit bases, so use 'type'
        meta = (mp_obj_t)&mp_type_type;
    } else {
        // use type of first base object
        meta = mp_obj_get_type(args[2]);
    }

    // TODO do proper metaclass resolution for multiple base objects

    // create the new class using a call to the meta object
    mp_obj_t meta_args[3];
    meta_args[0] = args[1]; // class name
    meta_args[1] = mp_obj_new_tuple(n_args - 2, args + 2); // tuple of bases
    meta_args[2] = class_locals; // dict of members
    mp_obj_t new_class = rt_call_function_n_kw(meta, 3, 0, meta_args);

    // store into cell if neede
    if (cell != mp_const_none) {
        mp_obj_cell_set(cell, new_class);
    }

    return new_class;
}

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin___build_class___obj, 2, mp_builtin___build_class__);

STATIC mp_obj_t mp_builtin___repl_print__(mp_obj_t o) {
    if (o != mp_const_none) {
        mp_obj_print(o, PRINT_REPR);
        printf("\n");
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin___repl_print___obj, mp_builtin___repl_print__);

mp_obj_t mp_builtin_abs(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(o_in);
        if (val < 0) {
            val = -val;
        }
        return MP_OBJ_NEW_SMALL_INT(val);
#if MICROPY_ENABLE_FLOAT
    } else if (MP_OBJ_IS_TYPE(o_in, &mp_type_float)) {
        mp_float_t value = mp_obj_float_get(o_in);
        // TODO check for NaN etc
        if (value < 0) {
            return mp_obj_new_float(-value);
        } else {
            return o_in;
        }
    } else if (MP_OBJ_IS_TYPE(o_in, &mp_type_complex)) {
        mp_float_t real, imag;
        mp_obj_complex_get(o_in, &real, &imag);
        return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(real*real + imag*imag));
#endif
    } else {
        assert(0);
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_abs_obj, mp_builtin_abs);

STATIC mp_obj_t mp_builtin_all(mp_obj_t o_in) {
    mp_obj_t iterable = rt_getiter(o_in);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
        if (!rt_is_true(item)) {
            return mp_const_false;
        }
    }
    return mp_const_true;
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_all_obj, mp_builtin_all);

STATIC mp_obj_t mp_builtin_any(mp_obj_t o_in) {
    mp_obj_t iterable = rt_getiter(o_in);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
        if (rt_is_true(item)) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_any_obj, mp_builtin_any);

STATIC mp_obj_t mp_builtin_callable(mp_obj_t o_in) {
    if (mp_obj_is_callable(o_in)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_callable_obj, mp_builtin_callable);

STATIC mp_obj_t mp_builtin_chr(mp_obj_t o_in) {
    int ord = mp_obj_get_int(o_in);
    if (0 <= ord && ord <= 0x10ffff) {
        byte str[1] = {ord};
        return mp_obj_new_str(str, 1, true);
    } else {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "chr() arg not in range(0x110000)"));
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_chr_obj, mp_builtin_chr);

STATIC mp_obj_t mp_builtin_dir(uint n_args, const mp_obj_t *args) {
    // TODO make this function more general and less of a hack

    mp_map_t *map = NULL;
    if (n_args == 0) {
        // make a list of names in the local name space
        map = rt_locals_get();
    } else { // n_args == 1
        // make a list of names in the given object
        if (MP_OBJ_IS_TYPE(args[0], &mp_type_module)) {
            map = mp_obj_module_get_globals(args[0]);
        } else {
            mp_obj_type_t *type;
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_type)) {
                type = args[0];
            } else {
                type = mp_obj_get_type(args[0]);
            }
            if (type->locals_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(type->locals_dict, &dict_type)) {
                map = mp_obj_dict_get_map(type->locals_dict);
            }
        }
    }

    mp_obj_t dir = mp_obj_new_list(0, NULL);
    if (map != NULL) {
        for (uint i = 0; i < map->alloc; i++) {
            if (map->table[i].key != MP_OBJ_NULL) {
                mp_obj_list_append(dir, map->table[i].key);
            }
        }
    }

    return dir;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_dir_obj, 0, 1, mp_builtin_dir);

STATIC mp_obj_t mp_builtin_divmod(mp_obj_t o1_in, mp_obj_t o2_in) {
    if (MP_OBJ_IS_SMALL_INT(o1_in) && MP_OBJ_IS_SMALL_INT(o2_in)) {
        mp_small_int_t i1 = MP_OBJ_SMALL_INT_VALUE(o1_in);
        mp_small_int_t i2 = MP_OBJ_SMALL_INT_VALUE(o2_in);
        mp_obj_t args[2];
        args[0] = MP_OBJ_NEW_SMALL_INT(i1 / i2);
        args[1] = MP_OBJ_NEW_SMALL_INT(i1 % i2);
        return rt_build_tuple(2, args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "unsupported operand type(s) for divmod(): '%s' and '%s'", mp_obj_get_type_str(o1_in), mp_obj_get_type_str(o2_in)));
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_divmod_obj, mp_builtin_divmod);

STATIC mp_obj_t mp_builtin_hash(mp_obj_t o_in) {
    // TODO hash will generally overflow small integer; can we safely truncate it?
    return mp_obj_new_int(mp_obj_hash(o_in));
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_hash_obj, mp_builtin_hash);

STATIC mp_obj_t mp_builtin_iter(mp_obj_t o_in) {
    return rt_getiter(o_in);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_iter_obj, mp_builtin_iter);

STATIC mp_obj_t mp_builtin_len(mp_obj_t o_in) {
    mp_obj_t len = mp_obj_len_maybe(o_in);
    if (len == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "object of type '%s' has no len()", mp_obj_get_type_str(o_in)));
    } else {
        return len;
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_len_obj, mp_builtin_len);

STATIC mp_obj_t mp_builtin_max(uint n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        mp_obj_t iterable = rt_getiter(args[0]);
        mp_obj_t max_obj = NULL;
        mp_obj_t item;
        while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
            if (max_obj == NULL || mp_obj_less(max_obj, item)) {
                max_obj = item;
            }
        }
        if (max_obj == NULL) {
            nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "max() arg is an empty sequence"));
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

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin_max_obj, 1, mp_builtin_max);

STATIC mp_obj_t mp_builtin_min(uint n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        mp_obj_t iterable = rt_getiter(args[0]);
        mp_obj_t min_obj = NULL;
        mp_obj_t item;
        while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
            if (min_obj == NULL || mp_obj_less(item, min_obj)) {
                min_obj = item;
            }
        }
        if (min_obj == NULL) {
            nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "min() arg is an empty sequence"));
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

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin_min_obj, 1, mp_builtin_min);

STATIC mp_obj_t mp_builtin_next(mp_obj_t o) {
    mp_obj_t ret = rt_iternext_allow_raise(o);
    if (ret == MP_OBJ_NULL) {
        nlr_jump(mp_obj_new_exception(&mp_type_StopIteration));
    } else {
        return ret;
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_next_obj, mp_builtin_next);

STATIC mp_obj_t mp_builtin_ord(mp_obj_t o_in) {
    uint len;
    const char *str = mp_obj_str_get_data(o_in, &len);
    if (len == 1) {
        // don't sign extend when converting to ord
        // TODO unicode
        return mp_obj_new_int(((const byte*)str)[0]);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "ord() expected a character, but string of length %d found", len));
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_ord_obj, mp_builtin_ord);

STATIC mp_obj_t mp_builtin_pow(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    switch (n_args) {
        case 2: return rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]);
        default: return rt_binary_op(RT_BINARY_OP_MODULO, rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]), args[2]); // TODO optimise...
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_pow_obj, 2, 3, mp_builtin_pow);

STATIC mp_obj_t mp_builtin_print(uint n_args, const mp_obj_t *args) {
    for (int i = 0; i < n_args; i++) {
        if (i > 0) {
            printf(" ");
        }
        mp_obj_print(args[i], PRINT_STR);
    }
    printf("\n");
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin_print_obj, 0, mp_builtin_print);

STATIC mp_obj_t mp_builtin_range(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 3);
    switch (n_args) {
        case 1: return mp_obj_new_range(0, mp_obj_get_int(args[0]), 1);
        case 2: return mp_obj_new_range(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), 1);
        default: return mp_obj_new_range(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_range_obj, 1, 3, mp_builtin_range);

STATIC mp_obj_t mp_builtin_repr(mp_obj_t o_in) {
    vstr_t *vstr = vstr_new();
    mp_obj_print_helper((void (*)(void *env, const char *fmt, ...))vstr_printf, vstr, o_in, PRINT_REPR);
    mp_obj_t s = mp_obj_new_str((byte*)vstr->buf, vstr->len, false);
    vstr_free(vstr);
    return s;
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_repr_obj, mp_builtin_repr);

STATIC mp_obj_t mp_builtin_sum(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    mp_obj_t value;
    switch (n_args) {
        case 1: value = mp_obj_new_int(0); break;
        default: value = args[1]; break;
    }
    mp_obj_t iterable = rt_getiter(args[0]);
    mp_obj_t item;
    while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
        value = rt_binary_op(RT_BINARY_OP_ADD, value, item);
    }
    return value;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_sum_obj, 1, 2, mp_builtin_sum);

STATIC mp_obj_t mp_builtin_sorted(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    assert(n_args >= 1);
    if (n_args > 1) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError,
                                          "must use keyword argument for key function"));
    }
    mp_obj_t self = list_type.make_new((mp_obj_t)&list_type, 1, 0, args);
    mp_obj_list_sort(1, &self, kwargs);

    return self;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_sorted_obj, 1, mp_builtin_sorted);

STATIC mp_obj_t mp_builtin_id(mp_obj_t o_in) {
    return mp_obj_new_int((machine_int_t)o_in);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_id_obj, mp_builtin_id);

STATIC mp_obj_t mp_builtin_getattr(mp_obj_t o_in, mp_obj_t attr) {
    assert(MP_OBJ_IS_QSTR(attr));
    return rt_load_attr(o_in, MP_OBJ_QSTR_VALUE(attr));
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_getattr_obj, mp_builtin_getattr);
