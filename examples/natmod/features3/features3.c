/* This example demonstrates the following features in a native module:
    - using types
    - using constant objects
    - creating dictionaries
*/

// Include the header file to get access to the MicroPython API.
#include "py/dynruntime.h"

// A function that returns a tuple of object types.
STATIC mp_obj_t get_types(void) {
    return mp_obj_new_tuple(9, ((mp_obj_t []) {
        MP_OBJ_FROM_PTR(&mp_type_type),
        MP_OBJ_FROM_PTR(&mp_type_NoneType),
        MP_OBJ_FROM_PTR(&mp_type_bool),
        MP_OBJ_FROM_PTR(&mp_type_int),
        MP_OBJ_FROM_PTR(&mp_type_str),
        MP_OBJ_FROM_PTR(&mp_type_bytes),
        MP_OBJ_FROM_PTR(&mp_type_tuple),
        MP_OBJ_FROM_PTR(&mp_type_list),
        MP_OBJ_FROM_PTR(&mp_type_dict),
    }));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_types_obj, get_types);

// A function that returns a tuple of constant objects.
STATIC mp_obj_t get_const_objects(void) {
    return mp_obj_new_tuple(5, ((mp_obj_t []) {
        mp_const_none,
        mp_const_false,
        mp_const_true,
        mp_const_empty_bytes,
        mp_const_empty_tuple,
    }));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_const_objects_obj, get_const_objects);

// A function that creates a dictionary from the given arguments.
STATIC mp_obj_t make_dict(size_t n_args, const mp_obj_t *args) {
    mp_obj_t dict = mp_obj_new_dict(n_args / 2);
    for (; n_args >= 2; n_args -= 2, args += 2) {
        mp_obj_dict_store(dict, args[0], args[1]);
    }
    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(make_dict_obj, 0, MP_OBJ_FUN_ARGS_MAX, make_dict);

// This is the entry point and is called when the module is imported.
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things.
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the functions available in the module's namespace.
    mp_store_global(MP_QSTR_make_dict, MP_OBJ_FROM_PTR(&make_dict_obj));
    mp_store_global(MP_QSTR_get_types, MP_OBJ_FROM_PTR(&get_types_obj));
    mp_store_global(MP_QSTR_get_const_objects, MP_OBJ_FROM_PTR(&get_const_objects_obj));

    // This must be last, it restores the globals dict.
    MP_DYNRUNTIME_INIT_EXIT
}
