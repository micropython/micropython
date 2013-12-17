#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"
#include "bc.h"

#include "map.h"
#include "obj.h"
#include "objprivate.h"
#include "builtin.h"

py_obj_t py_builtin___build_class__(py_obj_t o_class_fun, py_obj_t o_class_name) {
    // we differ from CPython: we set the new __locals__ object here
    py_map_t *old_locals = rt_get_map_locals();
    py_map_t *class_locals = py_map_new(MAP_QSTR, 0);
    rt_set_map_locals(class_locals);

    // call the class code
    rt_call_function_1(o_class_fun, (py_obj_t)0xdeadbeef);

    // restore old __locals__ object
    rt_set_map_locals(old_locals);

    // create and return the new class
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CLASS;
    o->u_class.locals = class_locals;
    return o;
}

py_obj_t py_builtin___import__(int n, py_obj_t *args) {
    printf("import:\n");
    for (int i = 0; i < n; i++) {
    printf("  ");
    py_obj_print(args[i]);
    printf("\n");
    }
    return py_const_none;
}

py_obj_t py_builtin___repl_print__(py_obj_t o) {
    if (o != py_const_none) {
        py_obj_print(o);
        printf("\n");
    }
    return py_const_none;
}

py_obj_t py_builtin_abs(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        py_small_int_t val = FROM_SMALL_INT(o_in);
        if (val < 0) {
            val = -val;
        }
        return TO_SMALL_INT(val);
#if MICROPY_ENABLE_FLOAT
    } else if (IS_O(o_in, O_FLOAT)) {
        py_obj_base_t *o = o_in;
        // TODO check for NaN etc
        if (o->u_float < 0) {
            return py_obj_new_float(-o->u_float);
        } else {
            return o_in;
        }
    } else if (IS_O(o_in, O_COMPLEX)) {
        py_obj_base_t *o = o_in;
        return py_obj_new_float(machine_sqrt(o->u_complex.real*o->u_complex.real + o->u_complex.imag*o->u_complex.imag));
#endif
    } else {
        assert(0);
        return py_const_none;
    }
}

py_obj_t py_builtin_all(py_obj_t o_in) {
    py_obj_t iterable = rt_getiter(o_in);
    py_obj_t item;
    while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
        if (!rt_is_true(item)) {
            return py_const_false;
        }
    }
    return py_const_true;
}

py_obj_t py_builtin_any(py_obj_t o_in) {
    py_obj_t iterable = rt_getiter(o_in);
    py_obj_t item;
    while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
        if (rt_is_true(item)) {
            return py_const_true;
        }
    }
    return py_const_false;
}

py_obj_t py_builtin_bool(int n_args, const py_obj_t *args) {
    switch (n_args) {
        case 0: return py_const_false;
        case 1: if (rt_is_true(args[0])) { return py_const_true; } else { return py_const_false; }
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "bool() takes at most 1 argument (%d given)", (void*)(machine_int_t)n_args, NULL));
    }
}

py_obj_t py_builtin_callable(py_obj_t o_in) {
    if (py_obj_is_callable(o_in)) {
        return py_const_true;
    } else {
        return py_const_false;
    }
}

#if MICROPY_ENABLE_FLOAT
py_obj_t py_builtin_complex(int n_args, const py_obj_t *args) {
    switch (n_args) {
        case 0:
            return py_obj_new_complex(0, 0);

        case 1:
            // TODO allow string as first arg
            if (IS_O(args[0], O_COMPLEX)) {
                return args[0];
            } else {
                return py_obj_new_complex(py_obj_get_float(args[0]), 0);
            }

        case 2:
        {
            py_float_t real, imag;
            if (IS_O(args[0], O_COMPLEX)) {
                py_obj_get_complex(args[0], &real, &imag);
            } else {
                real = py_obj_get_float(args[0]);
                imag = 0;
            }
            if (IS_O(args[1], O_COMPLEX)) {
                py_float_t real2, imag2;
                py_obj_get_complex(args[1], &real2, &imag2);
                real -= imag2;
                imag += real2;
            } else {
                imag += py_obj_get_float(args[1]);
            }
            return py_obj_new_complex(real, imag);
        }

        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "comlpex() takes at most 2 arguments (%d given)", (void*)(machine_int_t)n_args, NULL));
    }
}
#endif

py_obj_t py_builtin_chr(py_obj_t o_in) {
    int ord = py_obj_get_int(o_in);
    if (0 <= ord && ord <= 0x10ffff) {
        char *str = m_new(char, 2);
        str[0] = ord;
        str[1] = '\0';
        return py_obj_new_str(qstr_from_str_take(str));
    } else {
        nlr_jump(py_obj_new_exception_2(rt_q_ValueError, "chr() arg not in range(0x110000)", NULL, NULL));
    }
}

py_obj_t py_builtin_dict(void) {
    // TODO create from an iterable!
    return rt_build_map(0);
}

py_obj_t py_builtin_divmod(py_obj_t o1_in, py_obj_t o2_in) {
    if (IS_SMALL_INT(o1_in) && IS_SMALL_INT(o2_in)) {
        py_small_int_t i1 = FROM_SMALL_INT(o1_in);
        py_small_int_t i2 = FROM_SMALL_INT(o2_in);
        py_obj_t revs_args[2];
        revs_args[1] = TO_SMALL_INT(i1 / i2);
        revs_args[0] = TO_SMALL_INT(i1 % i2);
        return rt_build_tuple(2, revs_args);
    } else {
        nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "unsupported operand type(s) for divmod(): '%s' and '%s'", py_obj_get_type_str(o1_in), py_obj_get_type_str(o2_in)));
    }
}

py_obj_t py_builtin_hash(py_obj_t o_in) {
    // TODO hash will generally overflow small integer; can we safely truncate it?
    return py_obj_new_int(py_obj_hash(o_in));
}

py_obj_t py_builtin_iter(py_obj_t o_in) {
    return rt_getiter(o_in);
}

py_obj_t py_builtin_next(py_obj_t o_in) {
    return rt_gen_instance_next(o_in);
}

py_obj_t py_builtin_len(py_obj_t o_in) {
    py_small_int_t len = 0;
    if (IS_O(o_in, O_STR)) {
        py_obj_base_t *o = o_in;
        len = strlen(qstr_str(o->u_str));
    } else if (IS_O(o_in, O_TUPLE) || IS_O(o_in, O_LIST)) {
        py_obj_base_t *o = o_in;
        len = o->u_tuple_list.len;
    } else if (IS_O(o_in, O_MAP)) {
        py_obj_base_t *o = o_in;
        len = o->u_map.used;
    } else {
        nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "object of type '%s' has no len()", py_obj_get_type_str(o_in), NULL));
    }
    return TO_SMALL_INT(len);
}

py_obj_t py_builtin_list(int n_args, const py_obj_t *args) {
    switch (n_args) {
        case 0: return rt_build_list(0, NULL);
        case 1:
        {
            // make list from iterable
            py_obj_t iterable = rt_getiter(args[0]);
            py_obj_t list = rt_build_list(0, NULL);
            py_obj_t item;
            while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
                rt_list_append(list, item);
            }
            return list;
        }
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "list() takes at most 1 argument (%d given)", (void*)(machine_int_t)n_args, NULL));
    }
}

py_obj_t py_builtin_max(int n_args, const py_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        py_obj_t iterable = rt_getiter(args[0]);
        py_obj_t max_obj = NULL;
        py_obj_t item;
        while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
            if (max_obj == NULL || py_obj_less(max_obj, item)) {
                max_obj = item;
            }
        }
        if (max_obj == NULL) {
            nlr_jump(py_obj_new_exception_2(rt_q_ValueError, "max() arg is an empty sequence", NULL, NULL));
        }
        return max_obj;
    } else {
        // given many args
        py_obj_t max_obj = args[0];
        for (int i = 1; i < n_args; i++) {
            if (py_obj_less(max_obj, args[i])) {
                max_obj = args[i];
            }
        }
        return max_obj;
    }
}

py_obj_t py_builtin_min(int n_args, const py_obj_t *args) {
    if (n_args == 1) {
        // given an iterable
        py_obj_t iterable = rt_getiter(args[0]);
        py_obj_t min_obj = NULL;
        py_obj_t item;
        while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
            if (min_obj == NULL || py_obj_less(item, min_obj)) {
                min_obj = item;
            }
        }
        if (min_obj == NULL) {
            nlr_jump(py_obj_new_exception_2(rt_q_ValueError, "min() arg is an empty sequence", NULL, NULL));
        }
        return min_obj;
    } else {
        // given many args
        py_obj_t min_obj = args[0];
        for (int i = 1; i < n_args; i++) {
            if (py_obj_less(args[i], min_obj)) {
                min_obj = args[i];
            }
        }
        return min_obj;
    }
}

py_obj_t py_builtin_ord(py_obj_t o_in) {
    const char *str = qstr_str(py_obj_get_qstr(o_in));
    if (strlen(str) == 1) {
        return py_obj_new_int(str[0]);
    } else {
        nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "ord() expected a character, but string of length %d found", (void*)(machine_int_t)strlen(str), NULL));
    }
}

py_obj_t py_builtin_pow(int n_args, const py_obj_t *args) {
    switch (n_args) {
        case 2: return rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]);
        case 3: return rt_binary_op(RT_BINARY_OP_MODULO, rt_binary_op(RT_BINARY_OP_POWER, args[0], args[1]), args[2]); // TODO optimise...
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "pow expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args, NULL));
    }
}

py_obj_t py_builtin_print(int n_args, const py_obj_t *args) {
    for (int i = 0; i < n_args; i++) {
        if (i > 0) {
            printf(" ");
        }
        if (IS_O(args[i], O_STR)) {
            // special case, print string raw
            printf("%s", qstr_str(((py_obj_base_t*)args[i])->u_str));
        } else {
            // print the object Python style
            py_obj_print(args[i]);
        }
    }
    printf("\n");
    return py_const_none;
}

py_obj_t py_builtin_range(int n_args, const py_obj_t *args) {
    switch (n_args) {
        case 1: return py_obj_new_range(0, py_obj_get_int(args[0]), 1);
        case 2: return py_obj_new_range(py_obj_get_int(args[0]), py_obj_get_int(args[1]), 1);
        case 3: return py_obj_new_range(py_obj_get_int(args[0]), py_obj_get_int(args[1]), py_obj_get_int(args[2]));
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "range expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args, NULL));
    }
}

py_obj_t py_builtin_sum(int n_args, const py_obj_t *args) {
    py_obj_t value;
    switch (n_args) {
        case 1: value = py_obj_new_int(0); break;
        case 2: value = args[1]; break;
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "sum expected at most 2 arguments, got %d", (void*)(machine_int_t)n_args, NULL));
    }
    py_obj_t iterable = rt_getiter(args[0]);
    py_obj_t item;
    while ((item = rt_iternext(iterable)) != py_const_stop_iteration) {
        value = rt_binary_op(RT_BINARY_OP_ADD, value, item);
    }
    return value;
}
