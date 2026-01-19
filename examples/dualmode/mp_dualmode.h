// Dual-mode module support header
// Allows the same C source to be compiled as either a user C module or native module

#ifndef MP_DUALMODE_H
#define MP_DUALMODE_H

#ifdef MICROPY_ENABLE_DYNRUNTIME
// Building as a native module (.mpy)
#include "py/dynruntime.h"

// For native modules, we can't use static initialization with runtime values
// So we'll use a different approach - define attributes with macros

// Start module definition
#define DUALMODE_MODULE_GLOBALS_TABLE(module_name) \
    /* QSTRs need to be referenced */ \
    static void _qstr_refs(void) {

// Module table entry - just reference the QSTR
#define DUALMODE_TABLE_ENTRY(name, value) \
        (void)MP_QSTR_##name;

// End table and define attribute list
#define DUALMODE_REGISTER_MODULE(qstr_name, module_obj) \
        (void)MP_QSTR_##qstr_name; \
    } \
    /* Define the attributes using the macro approach */ \
    #define MODULE_ATTR_LIST

// Add attribute to list  
#define DUALMODE_END_MODULE() /* end */

// Type definition helpers
#define DUALMODE_DEFINE_CONST_FUN_OBJ_1(name, fun) \
    MP_DEFINE_CONST_FUN_OBJ_1(name, fun)
#define DUALMODE_DEFINE_CONST_FUN_OBJ_2(name, fun) \
    MP_DEFINE_CONST_FUN_OBJ_2(name, fun)
#define DUALMODE_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(name, min, max, fun) \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(name, min, max, fun)

#else
// Building as a user C module (compiled into firmware)
#include "py/runtime.h"
#include "py/obj.h"

// User C modules use compile-time QSTRs
#define DUALMODE_QSTR(name) MP_QSTR_##name

// Module table entry for user C modules (mp_rom_map_elem_t)
#define DUALMODE_TABLE_ENTRY(name, value) \
    { MP_ROM_QSTR(MP_QSTR_##name), value },

// Module table definition
#define DUALMODE_MODULE_GLOBALS_TABLE(module_name) \
    static const mp_rom_map_elem_t module_name##_globals_table[] = {

// Module dictionary and object definition
#define DUALMODE_REGISTER_MODULE(qstr_name, module_obj) \
    }; \
    static MP_DEFINE_CONST_DICT(module_obj##_globals, module_obj##_globals_table); \
    const mp_obj_module_t module_obj = { \
        .base = { &mp_type_module }, \
        .globals = (mp_obj_dict_t *)&module_obj##_globals, \
    }; \
    MP_REGISTER_MODULE(MP_QSTR_##qstr_name, module_obj);

// No-op for user C modules
#define DUALMODE_END_MODULE()

// Type definition helpers (same for both modes)
#define DUALMODE_DEFINE_CONST_FUN_OBJ_1(name, fun) \
    static MP_DEFINE_CONST_FUN_OBJ_1(name, fun)
#define DUALMODE_DEFINE_CONST_FUN_OBJ_2(name, fun) \
    static MP_DEFINE_CONST_FUN_OBJ_2(name, fun)
#define DUALMODE_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(name, min, max, fun) \
    static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(name, min, max, fun)

#endif // MICROPY_ENABLE_DYNRUNTIME

// Common helpers for both modes
#ifdef MICROPY_ENABLE_DYNRUNTIME
#define DUALMODE_ROM_PTR(ptr) MP_OBJ_FROM_PTR(ptr)
#define DUALMODE_ROM_INT(val) MP_OBJ_NEW_SMALL_INT(val)
#define DUALMODE_ROM_QSTR(qstr) MP_OBJ_NEW_QSTR(qstr)
#else
#define DUALMODE_ROM_PTR(ptr) MP_ROM_PTR(ptr)
#define DUALMODE_ROM_INT(val) MP_ROM_INT(val)
#define DUALMODE_ROM_QSTR(qstr) MP_ROM_QSTR(qstr)
#endif

#endif // MP_DUALMODE_H