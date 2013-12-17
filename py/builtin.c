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

py_obj_t py_builtin___repl_print__(py_obj_t o) {
    if (o != py_const_none) {
        py_obj_print(o);
        printf("\n");
    }
    return py_const_none;
}

py_obj_t py_builtin_print(int n_args, const py_obj_t* args) {
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
        assert(0);
    }
    return TO_SMALL_INT(len);
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

py_obj_t py_builtin_range(int n_args, const py_obj_t* args) {
    switch (n_args) {
        case 1: return py_obj_new_range(0, py_obj_get_int(args[0]), 1);
        case 2: return py_obj_new_range(py_obj_get_int(args[0]), py_obj_get_int(args[1]), 1);
        case 3: return py_obj_new_range(py_obj_get_int(args[0]), py_obj_get_int(args[1]), py_obj_get_int(args[2]));
        default: nlr_jump(py_obj_new_exception_2(rt_q_TypeError, "range expected at most 3 arguments, got %d", (void*)(machine_int_t)n_args, NULL));
    }
}
