// Example of a native module using a hybrid approach
// This shows a practical way to reduce boilerplate while working within native module constraints

#include "py/dynruntime.h"

// Reference the QSTRs we'll use so they get included in the generated header
// We need to use them in the code for the preprocessor to pick them up
void _qstr_refs(void) {
    (void)MP_QSTR_double;
    (void)MP_QSTR_add;
    (void)MP_QSTR_message;
    (void)MP_QSTR_MAGIC_NUMBER;
}

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
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_from_hybrid_module);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_message_obj, get_message);

// Define module attributes using macros for cleaner init
#define MODULE_ATTR_LIST \
    ATTR(double, MP_OBJ_FROM_PTR(&double_value_obj)) \
    ATTR(add, MP_OBJ_FROM_PTR(&add_values_obj)) \
    ATTR(message, MP_OBJ_FROM_PTR(&get_message_obj)) \
    ATTR(MAGIC_NUMBER, MP_OBJ_NEW_SMALL_INT(42))

// Module init function
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Register all module attributes using the macro
    #define ATTR(name, value) mp_store_global(MP_QSTR_##name, value);
    MODULE_ATTR_LIST
    #undef ATTR
    
    MP_DYNRUNTIME_INIT_EXIT
}