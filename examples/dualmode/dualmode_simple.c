// Simplified dual-mode module example
// This source can be compiled as either a user C module or native module

#include "mp_dualmode_v2.h"

// Function implementations are the same for both modes
static mp_obj_t simple_add(mp_obj_t a_obj, mp_obj_t b_obj) {
    mp_int_t a = mp_obj_get_int(a_obj);
    mp_int_t b = mp_obj_get_int(b_obj);
    return mp_obj_new_int(a + b);
}
static MP_DEFINE_CONST_FUN_OBJ_2(simple_add_obj, simple_add);

static mp_obj_t simple_hello(void) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_dualmode);
}
static MP_DEFINE_CONST_FUN_OBJ_0(simple_hello_obj, simple_hello);

// Ensure QSTRs are available for native module build
DUALMODE_QSTR_REF(dualmode_simple)
DUALMODE_QSTR_REF(add)
DUALMODE_QSTR_REF(hello)
DUALMODE_QSTR_REF(Hello_dualmode)

#ifdef MICROPY_ENABLE_DYNRUNTIME
// Native module implementation

// Use the macro approach from our previous work
#define MODULE_ATTR_LIST \
    ATTR(add, MP_OBJ_FROM_PTR(&simple_add_obj)) \
    ATTR(hello, MP_OBJ_FROM_PTR(&simple_hello_obj))

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    #define ATTR(name, value) mp_store_global(MP_QSTR_##name, value);
    MODULE_ATTR_LIST
    #undef ATTR
    
    MP_DYNRUNTIME_INIT_EXIT
}

#else
// User C module implementation

static const mp_rom_map_elem_t dualmode_simple_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dualmode_simple) },
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&simple_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_hello), MP_ROM_PTR(&simple_hello_obj) },
};
static MP_DEFINE_CONST_DICT(dualmode_simple_globals, dualmode_simple_globals_table);

const mp_obj_module_t dualmode_simple_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&dualmode_simple_globals,
};

// Register the module
MP_REGISTER_MODULE(MP_QSTR_dualmode_simple, dualmode_simple_module);

#endif // MICROPY_ENABLE_DYNRUNTIME