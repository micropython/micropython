#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"

#include "map.h"
#include "obj.h"
#include "objprivate.h"

py_obj_t py_obj_new_int(machine_int_t value) {
    return TO_SMALL_INT(value);
}

py_obj_t py_obj_new_const(const char *id) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CONST;
    o->id = id;
    return (py_obj_t)o;
}

py_obj_t py_obj_new_str(qstr qstr) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_STR;
    o->u_str = qstr;
    return (py_obj_t)o;
}

#if MICROPY_ENABLE_FLOAT
py_obj_t py_obj_new_float(py_float_t val) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FLOAT;
    o->u_float = val;
    return (py_obj_t)o;
}

py_obj_t py_obj_new_complex(py_float_t real, py_float_t imag) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_COMPLEX;
    o->u_complex.real = real;
    o->u_complex.imag = imag;
    return (py_obj_t)o;
}
#endif

py_obj_t py_obj_new_exception_0(qstr id) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_EXCEPTION_0;
    o->u_exc0.id = id;
    return (py_obj_t)o;
}

py_obj_t py_obj_new_exception_2(qstr id, const char *fmt, const char *s1, const char *s2) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_EXCEPTION_N;
    o->u_exc_n.id = id;
    o->u_exc_n.n_args = 3;
    o->u_exc_n.args = m_new(const void*, 3);
    o->u_exc_n.args[0] = fmt;
    o->u_exc_n.args[1] = s1;
    o->u_exc_n.args[2] = s2;
    return (py_obj_t)o;
}

// range is a class and instances are immutable sequence objects
py_obj_t py_obj_new_range(int start, int stop, int step) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_RANGE;
    o->u_range.start = start;
    o->u_range.stop = stop;
    o->u_range.step = step;
    return o;
}

py_obj_t py_obj_new_range_iterator(int cur, int stop, int step) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_RANGE_IT;
    o->u_range_it.cur = cur;
    o->u_range_it.stop = stop;
    o->u_range_it.step = step;
    return o;
}

py_obj_t py_obj_new_tuple_iterator(py_obj_base_t *tuple, int cur) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_TUPLE_IT;
    o->u_tuple_list_it.obj = tuple;
    o->u_tuple_list_it.cur = cur;
    return o;
}

py_obj_t py_obj_new_list_iterator(py_obj_base_t *list, int cur) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_LIST_IT;
    o->u_tuple_list_it.obj = list;
    o->u_tuple_list_it.cur = cur;
    return o;
}

py_obj_t py_obj_new_user(const py_user_info_t *info, machine_uint_t data1, machine_uint_t data2) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_USER;
    // TODO should probably parse the info to turn strings to qstr's, and wrap functions in O_FUN_N objects
    // that'll take up some memory.  maybe we can lazily do the O_FUN_N: leave it a ptr to a C function, and
    // only when the method is looked-up do we change that to the O_FUN_N object.
    o->u_user.info = info;
    o->u_user.data1 = data1;
    o->u_user.data2 = data2;
    return o;
}

const char *py_obj_get_type_str(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        return "int";
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_CONST:
                if (o == py_const_none) {
                    return "NoneType";
                } else {
                    return "bool";
                }
            case O_STR:
                return "str";
#if MICROPY_ENABLE_FLOAT
            case O_FLOAT:
                return "float";
#endif
            case O_FUN_0:
            case O_FUN_1:
            case O_FUN_2:
            case O_FUN_N:
            case O_FUN_VAR:
            case O_FUN_BC:
                return "function";
            case O_GEN_INSTANCE:
                return "generator";
            case O_TUPLE:
                return "tuple";
            case O_LIST:
                return "list";
            case O_TUPLE_IT:
                return "tuple_iterator";
            case O_LIST_IT:
                return "list_iterator";
            case O_SET:
                return "set";
            case O_MAP:
                return "dict";
            case O_OBJ:
            {
                py_map_elem_t *qn = py_qstr_map_lookup(o->u_obj.class->u_class.locals, qstr_from_str_static("__qualname__"), false);
                assert(qn != NULL);
                assert(IS_O(qn->value, O_STR));
                return qstr_str(((py_obj_base_t*)qn->value)->u_str);
            }
            case O_USER:
                return o->u_user.info->type_name;
            default:
                assert(0);
                return "UnknownType";
        }
    }
}

void printf_wrapper(void *env, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void py_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        print(env, "%d", (int)FROM_SMALL_INT(o_in));
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_CONST:
                print(env, "%s", o->id);
                break;
            case O_STR:
                // TODO need to escape chars etc
                print(env, "'%s'", qstr_str(o->u_str));
                break;
#if MICROPY_ENABLE_FLOAT
            case O_FLOAT:
                print(env, "%.8g", o->u_float);
                break;
            case O_COMPLEX:
                if (o->u_complex.real == 0) {
                    print(env, "%.8gj", o->u_complex.imag);
                } else {
                    print(env, "(%.8g+%.8gj)", o->u_complex.real, o->u_complex.imag);
                }
                break;
#endif
            case O_EXCEPTION_0:
                print(env, "%s", qstr_str(o->u_exc0.id));
                break;
            case O_EXCEPTION_N:
                print(env, "%s: ", qstr_str(o->u_exc_n.id));
                print(env, o->u_exc_n.args[0], o->u_exc_n.args[1], o->u_exc_n.args[2]);
                break;
            case O_GEN_INSTANCE:
                print(env, "<generator object 'fun-name' at %p>", o);
                break;
            case O_TUPLE:
                print(env, "(");
                for (int i = 0; i < o->u_tuple_list.len; i++) {
                    if (i > 0) {
                        print(env, ", ");
                    }
                    py_obj_print_helper(print, env, o->u_tuple_list.items[i]);
                }
                if (o->u_tuple_list.len == 1) {
                    print(env, ",");
                }
                print(env, ")");
                break;
            case O_LIST:
                print(env, "[");
                for (int i = 0; i < o->u_tuple_list.len; i++) {
                    if (i > 0) {
                        print(env, ", ");
                    }
                    py_obj_print_helper(print, env, o->u_tuple_list.items[i]);
                }
                print(env, "]");
                break;
            case O_SET:
            {
                bool first = true;
                print(env, "{");
                for (int i = 0; i < o->u_set.alloc; i++) {
                    if (o->u_set.table[i] != NULL) {
                        if (!first) {
                            print(env, ", ");
                        }
                        first = false;
                        py_obj_print_helper(print, env, o->u_set.table[i]);
                    }
                }
                print(env, "}");
                break;
            }
            case O_MAP:
            {
                bool first = true;
                print(env, "{");
                for (int i = 0; i < o->u_map.alloc; i++) {
                    if (o->u_map.table[i].key != NULL) {
                        if (!first) {
                            print(env, ", ");
                        }
                        first = false;
                        py_obj_print_helper(print, env, o->u_map.table[i].key);
                        print(env, ": ");
                        py_obj_print_helper(print, env, o->u_map.table[i].value);
                    }
                }
                print(env, "}");
                break;
            }
            case O_USER:
                if (o->u_user.info->print == NULL) {
                    print(env, "<unknown user object>");
                } else {
                    o->u_user.info->print(o_in);
                }
                break;
            default:
                print(env, "<? %d>", o->kind);
                assert(0);
        }
    }
}

void py_obj_print(py_obj_t o_in) {
    py_obj_print_helper(printf_wrapper, NULL, o_in);
}

bool py_obj_is_callable(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        return false;
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_FUN_0:
            case O_FUN_1:
            case O_FUN_2:
            case O_FUN_VAR:
            case O_FUN_N:
            case O_FUN_BC:
            case O_FUN_ASM:
            // what about O_CLASS, and an O_OBJ that has a __call__ method?
                return true;
            default:
                return false;
        }
    }
}

machine_int_t py_obj_hash(py_obj_t o_in) {
    if (o_in == py_const_false) {
        return 0; // needs to hash to same as the integer 0, since False==0
    } else if (o_in == py_const_true) {
        return 1; // needs to hash to same as the integer 1, since True==1
    } else if (IS_SMALL_INT(o_in)) {
        return FROM_SMALL_INT(o_in);
    } else if (IS_O(o_in, O_CONST)) {
        return (machine_int_t)o_in;
    } else if (IS_O(o_in, O_STR)) {
        return ((py_obj_base_t*)o_in)->u_str;
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
bool py_obj_equal(py_obj_t o1, py_obj_t o2) {
    if (o1 == o2) {
        return true;
    } else if (IS_SMALL_INT(o1) || IS_SMALL_INT(o2)) {
        if (IS_SMALL_INT(o1) && IS_SMALL_INT(o2)) {
            return false;
        } else {
            if (IS_SMALL_INT(o2)) {
                py_obj_t temp = o1; o1 = o2; o2 = temp;
            }
            // o1 is the SMALL_INT, o2 is not
            py_small_int_t val = FROM_SMALL_INT(o1);
            if (o2 == py_const_false) {
                return val == 0;
            } else if (o2 == py_const_true) {
                return val == 1;
            } else {
                return false;
            }
        }
    } else if (IS_O(o1, O_STR) && IS_O(o2, O_STR)) {
        return ((py_obj_base_t*)o1)->u_str == ((py_obj_base_t*)o2)->u_str;
    } else {
        assert(0);
        return false;
    }
}

machine_int_t py_obj_get_int(py_obj_t arg) {
    if (arg == py_const_false) {
        return 0;
    } else if (arg == py_const_true) {
        return 1;
    } else if (IS_SMALL_INT(arg)) {
        return FROM_SMALL_INT(arg);
    } else {
        assert(0);
        return 0;
    }
}

#if MICROPY_ENABLE_FLOAT
machine_float_t py_obj_get_float(py_obj_t arg) {
    if (arg == py_const_false) {
        return 0;
    } else if (arg == py_const_true) {
        return 1;
    } else if (IS_SMALL_INT(arg)) {
        return FROM_SMALL_INT(arg);
    } else if (IS_O(arg, O_FLOAT)) {
        return ((py_obj_base_t*)arg)->u_float;
    } else {
        assert(0);
        return 0;
    }
}

void py_obj_get_complex(py_obj_t arg, py_float_t *real, py_float_t *imag) {
    if (arg == py_const_false) {
        *real = 0;
        *imag = 0;
    } else if (arg == py_const_true) {
        *real = 1;
        *imag = 0;
    } else if (IS_SMALL_INT(arg)) {
        *real = FROM_SMALL_INT(arg);
        *imag = 0;
    } else if (IS_O(arg, O_FLOAT)) {
        *real = ((py_obj_base_t*)arg)->u_float;
        *imag = 0;
    } else if (IS_O(arg, O_COMPLEX)) {
        *real = ((py_obj_base_t*)arg)->u_complex.real;
        *imag = ((py_obj_base_t*)arg)->u_complex.imag;
    } else {
        assert(0);
        *real = 0;
        *imag = 0;
    }
}
#endif

qstr py_obj_get_qstr(py_obj_t arg) {
    if (IS_O(arg, O_STR)) {
        return ((py_obj_base_t*)arg)->u_str;
    } else {
        assert(0);
        return 0;
    }
}

py_obj_t *py_obj_get_array_fixed_n(py_obj_t o_in, machine_int_t n) {
    if (IS_O(o_in, O_TUPLE) || IS_O(o_in, O_LIST)) {
        py_obj_base_t *o = o_in;
        if (o->u_tuple_list.len != n) {
            nlr_jump(py_obj_new_exception_2(rt_q_IndexError, "requested length %d but object has length %d", (void*)n, (void*)o->u_tuple_list.len));
        }
        return o->u_tuple_list.items;
    } else {
        nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "object '%s' is not a tuple or list", py_obj_get_type_str(o_in), NULL));
    }
}

void py_user_get_data(py_obj_t o, machine_uint_t *data1, machine_uint_t *data2) {
    assert(IS_O(o, O_USER));
    if (data1 != NULL) {
        *data1 = ((py_obj_base_t*)o)->u_user.data1;
    }
    if (data2 != NULL) {
        *data2 = ((py_obj_base_t*)o)->u_user.data2;
    }
}

void py_user_set_data(py_obj_t o, machine_uint_t data1, machine_uint_t data2) {
    assert(IS_O(o, O_USER));
    ((py_obj_base_t*)o)->u_user.data1 = data1;
    ((py_obj_base_t*)o)->u_user.data2 = data2;
}

// temporary way of making C modules
// hack: use class to mimic a module

py_obj_t py_module_new(void) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CLASS;
    o->u_class.locals = py_map_new(MAP_QSTR, 0);
    return o;
}
