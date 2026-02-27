// Example of a native module using static definition pattern
// This demonstrates how native modules can use the same definition style as built-in modules

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

// Define module globals table
// For native modules, we need to use mp_rom_obj_t instead of mp_rom_map_elem_t
const mp_rom_obj_t static_module_globals_table[] = {
    // Use direct QSTR objects and values
    MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_static_module),
    MP_OBJ_NEW_QSTR(MP_QSTR_double), MP_ROM_PTR(&double_value_obj),
    MP_OBJ_NEW_QSTR(MP_QSTR_add), MP_ROM_PTR(&add_values_obj),
    MP_OBJ_NEW_QSTR(MP_QSTR_message), MP_ROM_PTR(&get_message_obj),
    MP_OBJ_NEW_QSTR(MP_QSTR_MAGIC_NUMBER), MP_OBJ_NEW_SMALL_INT(42),
};

// Store globals count for the generated init function
#define STATIC_MODULE_GLOBALS_COUNT (MP_ARRAY_SIZE(static_module_globals_table) / 2)

// This triggers the parser to generate an init function
// Native modules don't need the module object, just the registration
MP_REGISTER_MODULE(MP_QSTR_static_module, static_module);