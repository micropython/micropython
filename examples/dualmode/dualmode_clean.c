// Clean dual-mode module example
// This source can be compiled as either a user C module or native module

#ifdef MICROPY_ENABLE_DYNRUNTIME
// Native module build
#include "py/dynruntime.h"
#else
// User C module build  
#include "py/runtime.h"
#include "py/obj.h"
#endif

// Function implementations are the same for both modes
static mp_obj_t clean_add(mp_obj_t a_obj, mp_obj_t b_obj) {
    mp_int_t a = mp_obj_get_int(a_obj);
    mp_int_t b = mp_obj_get_int(b_obj);
    return mp_obj_new_int(a + b);
}
static MP_DEFINE_CONST_FUN_OBJ_2(clean_add_obj, clean_add);

static mp_obj_t clean_hello(void) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_Hello_dualmode_clean);
}
static MP_DEFINE_CONST_FUN_OBJ_0(clean_hello_obj, clean_hello);

#ifdef MICROPY_ENABLE_DYNRUNTIME
// ========== NATIVE MODULE IMPLEMENTATION ==========

// Define module attributes using macros
#define MODULE_ATTR_LIST \
    ATTR(add, MP_OBJ_FROM_PTR(&clean_add_obj)) \
    ATTR(hello, MP_OBJ_FROM_PTR(&clean_hello_obj)) \
    ATTR(VERSION, MP_OBJ_NEW_SMALL_INT(200))

// Reference QSTRs
void _qstr_refs(void) {
    (void)MP_QSTR_dualmode_clean;
    (void)MP_QSTR_add;
    (void)MP_QSTR_hello;
    (void)MP_QSTR_VERSION;
    (void)MP_QSTR_Hello_dualmode_clean;
}

// Module init function
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    #define ATTR(name, value) mp_store_global(MP_QSTR_##name, value);
    MODULE_ATTR_LIST
    #undef ATTR
    
    MP_DYNRUNTIME_INIT_EXIT
}

#else
// ========== USER C MODULE IMPLEMENTATION ==========

static const mp_rom_map_elem_t dualmode_clean_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dualmode_clean) },
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&clean_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_hello), MP_ROM_PTR(&clean_hello_obj) },
    { MP_ROM_QSTR(MP_QSTR_VERSION), MP_ROM_INT(200) },
};
static MP_DEFINE_CONST_DICT(dualmode_clean_globals, dualmode_clean_globals_table);

const mp_obj_module_t dualmode_clean_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&dualmode_clean_globals,
};

// Note: parser will see this and it's OK for user C module builds
MP_REGISTER_MODULE(MP_QSTR_dualmode_clean, dualmode_clean_module);

#endif // MICROPY_ENABLE_DYNRUNTIME