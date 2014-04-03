#include <stdlib.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "builtintables.h"
#include "objarray.h"

// builtins
// we put these tables in ROM because they're always needed and take up quite a bit of room in RAM
// in fact, it uses less ROM here in table form than the equivalent in code form initialising a dynamic mp_map_t object in RAM
// at the moment it's a linear table, but we could convert it to a const mp_map_t table with a simple preprocessing script

typedef struct _mp_builtin_elem_t {
    qstr qstr;
    mp_obj_t elem;
} mp_builtin_elem_t;

STATIC const mp_builtin_elem_t builtin_object_table[] = {
    // built-in core functions
    { MP_QSTR___build_class__, (mp_obj_t)&mp_builtin___build_class___obj },
    { MP_QSTR___import__, (mp_obj_t)&mp_builtin___import___obj },
    { MP_QSTR___repl_print__, (mp_obj_t)&mp_builtin___repl_print___obj },

    // built-in types
    { MP_QSTR_bool, (mp_obj_t)&mp_type_bool },
    { MP_QSTR_bytes, (mp_obj_t)&mp_type_bytes },
#if MICROPY_ENABLE_FLOAT
    { MP_QSTR_complex, (mp_obj_t)&mp_type_complex },
#endif
    { MP_QSTR_dict, (mp_obj_t)&mp_type_dict },
    { MP_QSTR_enumerate, (mp_obj_t)&mp_type_enumerate },
    { MP_QSTR_filter, (mp_obj_t)&mp_type_filter },
#if MICROPY_ENABLE_FLOAT
    { MP_QSTR_float, (mp_obj_t)&mp_type_float },
#endif
    { MP_QSTR_int, (mp_obj_t)&mp_type_int },
    { MP_QSTR_list, (mp_obj_t)&mp_type_list },
    { MP_QSTR_map, (mp_obj_t)&mp_type_map },
    { MP_QSTR_object, (mp_obj_t)&mp_type_object },
    { MP_QSTR_set, (mp_obj_t)&mp_type_set },
    { MP_QSTR_str, (mp_obj_t)&mp_type_str },
    { MP_QSTR_super, (mp_obj_t)&mp_type_super },
    { MP_QSTR_tuple, (mp_obj_t)&mp_type_tuple },
    { MP_QSTR_type, (mp_obj_t)&mp_type_type },
    { MP_QSTR_zip, (mp_obj_t)&mp_type_zip },

    { MP_QSTR_classmethod, (mp_obj_t)&mp_type_classmethod },
    { MP_QSTR_staticmethod, (mp_obj_t)&mp_type_staticmethod },

    // built-in objects
    { MP_QSTR_Ellipsis, (mp_obj_t)&mp_const_ellipsis_obj },

    // built-in user functions
    { MP_QSTR_abs, (mp_obj_t)&mp_builtin_abs_obj },
    { MP_QSTR_all, (mp_obj_t)&mp_builtin_all_obj },
    { MP_QSTR_any, (mp_obj_t)&mp_builtin_any_obj },
    { MP_QSTR_callable, (mp_obj_t)&mp_builtin_callable_obj },
    { MP_QSTR_chr, (mp_obj_t)&mp_builtin_chr_obj },
    { MP_QSTR_dir, (mp_obj_t)&mp_builtin_dir_obj },
    { MP_QSTR_divmod, (mp_obj_t)&mp_builtin_divmod_obj },
    { MP_QSTR_eval, (mp_obj_t)&mp_builtin_eval_obj },
    { MP_QSTR_exec, (mp_obj_t)&mp_builtin_exec_obj },
    { MP_QSTR_getattr, (mp_obj_t)&mp_builtin_getattr_obj },
    { MP_QSTR_hash, (mp_obj_t)&mp_builtin_hash_obj },
    { MP_QSTR_id, (mp_obj_t)&mp_builtin_id_obj },
    { MP_QSTR_isinstance, (mp_obj_t)&mp_builtin_isinstance_obj },
    { MP_QSTR_issubclass, (mp_obj_t)&mp_builtin_issubclass_obj },
    { MP_QSTR_iter, (mp_obj_t)&mp_builtin_iter_obj },
    { MP_QSTR_len, (mp_obj_t)&mp_builtin_len_obj },
    { MP_QSTR_max, (mp_obj_t)&mp_builtin_max_obj },
    { MP_QSTR_min, (mp_obj_t)&mp_builtin_min_obj },
    { MP_QSTR_next, (mp_obj_t)&mp_builtin_next_obj },
    { MP_QSTR_ord, (mp_obj_t)&mp_builtin_ord_obj },
    { MP_QSTR_pow, (mp_obj_t)&mp_builtin_pow_obj },
    { MP_QSTR_print, (mp_obj_t)&mp_builtin_print_obj },
    { MP_QSTR_range, (mp_obj_t)&mp_builtin_range_obj },
    { MP_QSTR_repr, (mp_obj_t)&mp_builtin_repr_obj },
    { MP_QSTR_sorted, (mp_obj_t)&mp_builtin_sorted_obj },
    { MP_QSTR_sum, (mp_obj_t)&mp_builtin_sum_obj },
    { MP_QSTR_bytearray, (mp_obj_t)&mp_builtin_bytearray_obj },

    // built-in exceptions
    { MP_QSTR_BaseException, (mp_obj_t)&mp_type_BaseException },
    { MP_QSTR_ArithmeticError, (mp_obj_t)&mp_type_ArithmeticError },
    { MP_QSTR_AssertionError, (mp_obj_t)&mp_type_AssertionError },
    { MP_QSTR_AttributeError, (mp_obj_t)&mp_type_AttributeError },
    { MP_QSTR_EOFError, (mp_obj_t)&mp_type_EOFError },
    { MP_QSTR_Exception, (mp_obj_t)&mp_type_Exception },
    { MP_QSTR_GeneratorExit, (mp_obj_t)&mp_type_GeneratorExit },
    { MP_QSTR_IOError, (mp_obj_t)&mp_type_IOError },
    { MP_QSTR_ImportError, (mp_obj_t)&mp_type_ImportError },
    { MP_QSTR_IndentationError, (mp_obj_t)&mp_type_IndentationError },
    { MP_QSTR_IndexError, (mp_obj_t)&mp_type_IndexError },
    { MP_QSTR_KeyError, (mp_obj_t)&mp_type_KeyError },
    { MP_QSTR_LookupError, (mp_obj_t)&mp_type_LookupError },
    { MP_QSTR_MemoryError, (mp_obj_t)&mp_type_MemoryError },
    { MP_QSTR_NameError, (mp_obj_t)&mp_type_NameError },
    { MP_QSTR_NotImplementedError, (mp_obj_t)&mp_type_NotImplementedError },
    { MP_QSTR_OSError, (mp_obj_t)&mp_type_OSError },
    { MP_QSTR_OverflowError, (mp_obj_t)&mp_type_OverflowError },
    { MP_QSTR_RuntimeError, (mp_obj_t)&mp_type_RuntimeError },
    { MP_QSTR_StopIteration, (mp_obj_t)&mp_type_StopIteration },
    { MP_QSTR_SyntaxError, (mp_obj_t)&mp_type_SyntaxError },
    { MP_QSTR_SystemError, (mp_obj_t)&mp_type_SystemError },
    { MP_QSTR_TypeError, (mp_obj_t)&mp_type_TypeError },
    { MP_QSTR_ValueError, (mp_obj_t)&mp_type_ValueError },
    { MP_QSTR_ZeroDivisionError, (mp_obj_t)&mp_type_ZeroDivisionError },
    // Somehow CPython managed to have OverflowError not inherit from ValueError ;-/
    // TODO: For MICROPY_CPYTHON_COMPAT==0 use ValueError to avoid exc proliferation

    // Extra builtins as defined by a port
    MICROPY_EXTRA_BUILTINS

    { MP_QSTR_, MP_OBJ_NULL }, // end of list sentinel
};

STATIC const mp_builtin_elem_t builtin_module_table[] = {
    { MP_QSTR_micropython, (mp_obj_t)&mp_module_micropython },

    { MP_QSTR_array, (mp_obj_t)&mp_module_array },
#if MICROPY_ENABLE_MOD_IO
    { MP_QSTR_io, (mp_obj_t)&mp_module_io },
#endif
    { MP_QSTR_collections, (mp_obj_t)&mp_module_collections },

#if MICROPY_ENABLE_FLOAT
    { MP_QSTR_math, (mp_obj_t)&mp_module_math },
#endif

    // extra builtin modules as defined by a port
    MICROPY_EXTRA_BUILTIN_MODULES

    { MP_QSTR_, MP_OBJ_NULL }, // end of list sentinel
};

STATIC mp_obj_t mp_builtin_tables_lookup(const mp_builtin_elem_t *table, qstr q) {
    for (; table->qstr != MP_QSTR_; table++) {
        if (table->qstr == q) {
            return table->elem;
        }
    }
    return MP_OBJ_NULL;
}

mp_obj_t mp_builtin_tables_lookup_object(qstr q) {
    return mp_builtin_tables_lookup(&builtin_object_table[0], q);
}

mp_obj_t mp_builtin_tables_lookup_module(qstr q) {
    return mp_builtin_tables_lookup(&builtin_module_table[0], q);
}
