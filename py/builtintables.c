/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "builtintables.h"
#include "objarray.h"

STATIC const mp_map_elem_t mp_builtin_object_table[] = {
    // built-in core functions
    { MP_OBJ_NEW_QSTR(MP_QSTR___build_class__), (mp_obj_t)&mp_builtin___build_class___obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___import__), (mp_obj_t)&mp_builtin___import___obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___repl_print__), (mp_obj_t)&mp_builtin___repl_print___obj },

    // built-in types
    { MP_OBJ_NEW_QSTR(MP_QSTR_bool), (mp_obj_t)&mp_type_bool },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bytes), (mp_obj_t)&mp_type_bytes },
#if MICROPY_PY_BUILTINS_BYTEARRAY
    { MP_OBJ_NEW_QSTR(MP_QSTR_bytearray), (mp_obj_t)&mp_type_bytearray },
#endif
#if MICROPY_PY_BUILTINS_COMPLEX
    { MP_OBJ_NEW_QSTR(MP_QSTR_complex), (mp_obj_t)&mp_type_complex },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_dict), (mp_obj_t)&mp_type_dict },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enumerate), (mp_obj_t)&mp_type_enumerate },
    { MP_OBJ_NEW_QSTR(MP_QSTR_filter), (mp_obj_t)&mp_type_filter },
#if MICROPY_PY_BUILTINS_FLOAT
    { MP_OBJ_NEW_QSTR(MP_QSTR_float), (mp_obj_t)&mp_type_float },
#endif
#if MICROPY_PY_BUILTINS_FROZENSET
    { MP_OBJ_NEW_QSTR(MP_QSTR_frozenset), (mp_obj_t)&mp_type_frozenset },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_int), (mp_obj_t)&mp_type_int },
    { MP_OBJ_NEW_QSTR(MP_QSTR_list), (mp_obj_t)&mp_type_list },
    { MP_OBJ_NEW_QSTR(MP_QSTR_map), (mp_obj_t)&mp_type_map },
    { MP_OBJ_NEW_QSTR(MP_QSTR_object), (mp_obj_t)&mp_type_object },
#if MICROPY_PY_BUILTINS_PROPERTY
    { MP_OBJ_NEW_QSTR(MP_QSTR_property), (mp_obj_t)&mp_type_property },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_range), (mp_obj_t)&mp_type_range },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reversed), (mp_obj_t)&mp_type_reversed },
#if MICROPY_PY_BUILTINS_SET
    { MP_OBJ_NEW_QSTR(MP_QSTR_set), (mp_obj_t)&mp_type_set },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_str), (mp_obj_t)&mp_type_str },
    { MP_OBJ_NEW_QSTR(MP_QSTR_super), (mp_obj_t)&mp_type_super },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tuple), (mp_obj_t)&mp_type_tuple },
    { MP_OBJ_NEW_QSTR(MP_QSTR_type), (mp_obj_t)&mp_type_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_zip), (mp_obj_t)&mp_type_zip },

    { MP_OBJ_NEW_QSTR(MP_QSTR_classmethod), (mp_obj_t)&mp_type_classmethod },
    { MP_OBJ_NEW_QSTR(MP_QSTR_staticmethod), (mp_obj_t)&mp_type_staticmethod },

    // built-in objects
    { MP_OBJ_NEW_QSTR(MP_QSTR_Ellipsis), (mp_obj_t)&mp_const_ellipsis_obj },

    // built-in user functions
    { MP_OBJ_NEW_QSTR(MP_QSTR_abs), (mp_obj_t)&mp_builtin_abs_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_all), (mp_obj_t)&mp_builtin_all_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&mp_builtin_any_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bin), (mp_obj_t)&mp_builtin_bin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callable), (mp_obj_t)&mp_builtin_callable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_chr), (mp_obj_t)&mp_builtin_chr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dir), (mp_obj_t)&mp_builtin_dir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_divmod), (mp_obj_t)&mp_builtin_divmod_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_eval), (mp_obj_t)&mp_builtin_eval_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_exec), (mp_obj_t)&mp_builtin_exec_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getattr), (mp_obj_t)&mp_builtin_getattr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_globals), (mp_obj_t)&mp_builtin_globals_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hasattr), (mp_obj_t)&mp_builtin_hasattr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hash), (mp_obj_t)&mp_builtin_hash_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hex), (mp_obj_t)&mp_builtin_hex_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_id), (mp_obj_t)&mp_builtin_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isinstance), (mp_obj_t)&mp_builtin_isinstance_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_issubclass), (mp_obj_t)&mp_builtin_issubclass_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_iter), (mp_obj_t)&mp_builtin_iter_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_len), (mp_obj_t)&mp_builtin_len_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_locals), (mp_obj_t)&mp_builtin_locals_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&mp_builtin_max_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&mp_builtin_min_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_next), (mp_obj_t)&mp_builtin_next_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_oct), (mp_obj_t)&mp_builtin_oct_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ord), (mp_obj_t)&mp_builtin_ord_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pow), (mp_obj_t)&mp_builtin_pow_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_print), (mp_obj_t)&mp_builtin_print_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repr), (mp_obj_t)&mp_builtin_repr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sorted), (mp_obj_t)&mp_builtin_sorted_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&mp_builtin_sum_obj },

    // built-in exceptions
    { MP_OBJ_NEW_QSTR(MP_QSTR_BaseException), (mp_obj_t)&mp_type_BaseException },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ArithmeticError), (mp_obj_t)&mp_type_ArithmeticError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AssertionError), (mp_obj_t)&mp_type_AssertionError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AttributeError), (mp_obj_t)&mp_type_AttributeError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EOFError), (mp_obj_t)&mp_type_EOFError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Exception), (mp_obj_t)&mp_type_Exception },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GeneratorExit), (mp_obj_t)&mp_type_GeneratorExit },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IOError), (mp_obj_t)&mp_type_IOError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ImportError), (mp_obj_t)&mp_type_ImportError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IndentationError), (mp_obj_t)&mp_type_IndentationError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IndexError), (mp_obj_t)&mp_type_IndexError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_KeyError), (mp_obj_t)&mp_type_KeyError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LookupError), (mp_obj_t)&mp_type_LookupError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MemoryError), (mp_obj_t)&mp_type_MemoryError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NameError), (mp_obj_t)&mp_type_NameError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NotImplementedError), (mp_obj_t)&mp_type_NotImplementedError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OSError), (mp_obj_t)&mp_type_OSError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OverflowError), (mp_obj_t)&mp_type_OverflowError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RuntimeError), (mp_obj_t)&mp_type_RuntimeError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_StopIteration), (mp_obj_t)&mp_type_StopIteration },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SyntaxError), (mp_obj_t)&mp_type_SyntaxError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SystemError), (mp_obj_t)&mp_type_SystemError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SystemExit), (mp_obj_t)&mp_type_SystemExit },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TypeError), (mp_obj_t)&mp_type_TypeError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ValueError), (mp_obj_t)&mp_type_ValueError },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ZeroDivisionError), (mp_obj_t)&mp_type_ZeroDivisionError },
    // Somehow CPython managed to have OverflowError not inherit from ValueError ;-/
    // TODO: For MICROPY_CPYTHON_COMPAT==0 use ValueError to avoid exc proliferation

    // Extra builtins as defined by a port
    MICROPY_PORT_BUILTINS
};

const mp_obj_dict_t mp_builtin_object_dict_obj = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_builtin_object_table),
        .alloc = MP_ARRAY_SIZE(mp_builtin_object_table),
        .table = (mp_map_elem_t*)mp_builtin_object_table,
    },
};

STATIC const mp_map_elem_t mp_builtin_module_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___main__), (mp_obj_t)&mp_module___main__ },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micropython), (mp_obj_t)&mp_module_micropython },

#if MICROPY_PY_ARRAY
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&mp_module_array },
#endif
#if MICROPY_PY_IO
    { MP_OBJ_NEW_QSTR(MP_QSTR__io), (mp_obj_t)&mp_module_io },
#endif
#if MICROPY_PY_COLLECTIONS
    { MP_OBJ_NEW_QSTR(MP_QSTR__collections), (mp_obj_t)&mp_module_collections },
#endif
#if MICROPY_PY_STRUCT
    { MP_OBJ_NEW_QSTR(MP_QSTR_struct), (mp_obj_t)&mp_module_struct },
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#if MICROPY_PY_MATH
    { MP_OBJ_NEW_QSTR(MP_QSTR_math), (mp_obj_t)&mp_module_math },
#endif
#if MICROPY_PY_CMATH
    { MP_OBJ_NEW_QSTR(MP_QSTR_cmath), (mp_obj_t)&mp_module_cmath },
#endif
#endif
#if MICROPY_PY_SYS
    { MP_OBJ_NEW_QSTR(MP_QSTR_sys), (mp_obj_t)&mp_module_sys },
#endif
#if MICROPY_PY_GC && MICROPY_ENABLE_GC
    { MP_OBJ_NEW_QSTR(MP_QSTR_gc), (mp_obj_t)&mp_module_gc },
#endif

    // extmod modules

#if MICROPY_PY_UCTYPES
    { MP_OBJ_NEW_QSTR(MP_QSTR_uctypes), (mp_obj_t)&mp_module_uctypes },
#endif
#if MICROPY_PY_ZLIBD
    { MP_OBJ_NEW_QSTR(MP_QSTR_zlibd), (mp_obj_t)&mp_module_zlibd },
#endif

    // extra builtin modules as defined by a port
    MICROPY_PORT_BUILTIN_MODULES
};

const mp_obj_dict_t mp_builtin_module_dict_obj = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_builtin_module_table),
        .alloc = MP_ARRAY_SIZE(mp_builtin_module_table),
        .table = (mp_map_elem_t*)mp_builtin_module_table,
    },
};
