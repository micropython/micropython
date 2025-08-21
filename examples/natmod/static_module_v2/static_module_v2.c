// Example of a native module using static definition pattern with helper macro
// This demonstrates a working approach for native modules

#include "py/dynruntime.h"

// Simple function that doubles its input
static mp_obj_t double_value(mp_obj_t x_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    return mp_obj_new_int(x * 2);
}
static MP_DEFINE_CONST_FUN_OBJ_1(double_value_obj, double_value);

// Function that adds two numbers
static mp_obj_t add_values(mp_obj_t a_obj, mp_obj_t b_obj) {
    mp_int_t a = mp_obj_get_int(a_obj);
    mp_int_t b = mp_obj_get_int(b_obj);
    return mp_obj_new_int(a + b);
}
static MP_DEFINE_CONST_FUN_OBJ_2(add_values_obj, add_values);

// Function that returns a string
static mp_obj_t get_message(void) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_from_static_module);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_message_obj, get_message);

// Define module globals table using mp_rom_map_elem_t
// This works because the init function will convert at runtime
static const mp_rom_map_elem_t static_module_v2_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_static_module_v2) },
    { MP_ROM_QSTR(MP_QSTR_double), MP_ROM_PTR(&double_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&add_values_obj) },
    { MP_ROM_QSTR(MP_QSTR_message), MP_ROM_PTR(&get_message_obj) },
    { MP_ROM_QSTR(MP_QSTR_MAGIC_NUMBER), MP_ROM_INT(42) },
};

// Manual init function that uses our helper macro
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Use the helper macro to register all globals
    MP_DYNRUNTIME_INIT_STATIC_MODULE(static_module_v2_globals_table);
    
    MP_DYNRUNTIME_INIT_EXIT
}