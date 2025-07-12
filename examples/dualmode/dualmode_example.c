// Example dual-mode module that can be compiled as either:
// 1. A user C module (compiled into firmware)
// 2. A native module (.mpy file)

#include "mp_dualmode.h"

// Function that adds two numbers
static mp_obj_t dualmode_add(mp_obj_t a_obj, mp_obj_t b_obj) {
    mp_int_t a = mp_obj_get_int(a_obj);
    mp_int_t b = mp_obj_get_int(b_obj);
    return mp_obj_new_int(a + b);
}
DUALMODE_DEFINE_CONST_FUN_OBJ_2(dualmode_add_obj, dualmode_add);

// Function that returns a string
static mp_obj_t dualmode_hello(void) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_from_dualmode);
}
DUALMODE_DEFINE_CONST_FUN_OBJ_0(dualmode_hello_obj, dualmode_hello);

// Function that doubles a value
static mp_obj_t dualmode_double(mp_obj_t x_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    return mp_obj_new_int(x * 2);
}
DUALMODE_DEFINE_CONST_FUN_OBJ_1(dualmode_double_obj, dualmode_double);

// Module globals table - works for both modes
DUALMODE_MODULE_GLOBALS_TABLE(dualmode_example)
    DUALMODE_TABLE_ENTRY(__name__, DUALMODE_ROM_QSTR(MP_QSTR_dualmode_example))
    DUALMODE_TABLE_ENTRY(add, DUALMODE_ROM_PTR(&dualmode_add_obj))
    DUALMODE_TABLE_ENTRY(hello, DUALMODE_ROM_PTR(&dualmode_hello_obj))
    DUALMODE_TABLE_ENTRY(double, DUALMODE_ROM_PTR(&dualmode_double_obj))
    DUALMODE_TABLE_ENTRY(VERSION, DUALMODE_ROM_INT(100))
DUALMODE_REGISTER_MODULE(dualmode_example, dualmode_example)
    // QSTR references for native module build
    (void)MP_QSTR_add;
    (void)MP_QSTR_hello;
    (void)MP_QSTR_double;
    (void)MP_QSTR_VERSION;
    (void)MP_QSTR_Hello_from_dualmode;
DUALMODE_END_MODULE()

// For native module build, we need the mpy_init function
#ifdef MICROPY_ENABLE_DYNRUNTIME
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Register all module attributes from the table
    const mp_rom_obj_t *table = dualmode_example_globals_table;
    for (size_t i = 0; i < MP_ARRAY_SIZE(dualmode_example_globals_table); i += 2) {
        qstr key = MP_OBJ_QSTR_VALUE(table[i]);
        if (key != MP_QSTR___name__) {
            mp_store_global(key, table[i + 1]);
        }
    }
    
    MP_DYNRUNTIME_INIT_EXIT
}
#endif