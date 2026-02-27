// Example of a native module using auto-generation approach
// This approach uses code generation to create the module registration

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
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_from_autogen_module);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_message_obj, get_message);

// Module attributes table for auto-generation
// This would be processed by a build script to generate the init function
#ifdef MODULE_GEN_TABLE
MODULE_BEGIN(static_module_autogen)
MODULE_FUNCTION(double, double_value_obj)
MODULE_FUNCTION(add, add_values_obj)
MODULE_FUNCTION(message, get_message_obj)
MODULE_CONST(MAGIC_NUMBER, 42)
MODULE_END()
#endif

// Include the auto-generated init function
// This needs to be at the end so all symbols are already defined
#ifndef MODULE_GEN_TABLE
#include "module_init_generated.h"
#endif