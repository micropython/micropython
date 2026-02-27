/*
 * MicroPython Dual-Mode Module Support
 * 
 * This header enables C modules to be compiled as both:
 * - User C modules (compiled into firmware)
 * - Native modules (dynamic .mpy files)
 * 
 * The same source code can be used for both contexts with minimal changes.
 */

#ifndef MICROPY_INCLUDED_MP_DUALMODE_H
#define MICROPY_INCLUDED_MP_DUALMODE_H

#ifdef MICROPY_ENABLE_DYNRUNTIME
// ========== Native Module Mode ==========
#include "py/dynruntime.h"

// Helper macro to define module globals table for native modules
// This stores pairs of QSTR/value directly
#define MP_DUALMODE_MODULE_GLOBALS_TABLE(module_name) \
    const mp_rom_obj_t module_name##_globals_table[]

// Helper to add an entry to the globals table
#define MP_DUALMODE_MODULE_ATTR(qstr_name, value) \
    MP_OBJ_NEW_QSTR(qstr_name), value,

// End the globals table definition
#define MP_DUALMODE_MODULE_END() \
    ;

// Calculate the number of entries in the globals table
#define MP_DUALMODE_GLOBALS_COUNT(module_name) \
    (MP_ARRAY_SIZE(module_name##_globals_table) / 2)

// Helper macro to register all module globals from a static table
#define MP_DUALMODE_REGISTER_MODULE_GLOBALS(module_name) \
    do { \
        const mp_rom_obj_t *table = module_name##_globals_table; \
        size_t count = MP_DUALMODE_GLOBALS_COUNT(module_name); \
        for (size_t i = 0; i < count * 2; i += 2) { \
            qstr key = MP_OBJ_QSTR_VALUE(table[i]); \
            if (key != MP_QSTR___name__) { \
                mp_store_global(key, table[i + 1]); \
            } \
        } \
    } while (0)

// Macro to define the mpy_init function
#define MP_DUALMODE_INIT_MODULE(module_name) \
    mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) { \
        MP_DYNRUNTIME_INIT_ENTRY \
        MP_DUALMODE_REGISTER_MODULE_GLOBALS(module_name); \
        MP_DYNRUNTIME_INIT_EXIT \
    }

// In native module mode, MP_REGISTER_MODULE does nothing
#define MP_REGISTER_MODULE(name, obj)

// Type compatibility macros
#define MP_DUALMODE_ROM_QSTR(q) MP_OBJ_NEW_QSTR(q)
#define MP_DUALMODE_ROM_INT(i) MP_OBJ_NEW_SMALL_INT(i)
#define MP_DUALMODE_ROM_PTR(p) MP_ROM_PTR(p)

#else
// ========== User C Module Mode ==========
#include "py/runtime.h"
#include "py/obj.h"

// For user C modules, use standard mp_rom_map_elem_t
#define MP_DUALMODE_MODULE_GLOBALS_TABLE(module_name) \
    static const mp_rom_map_elem_t module_name##_globals_table[]

// Helper to add an entry to the globals table  
#define MP_DUALMODE_MODULE_ATTR(qstr_name, value) \
    { MP_ROM_QSTR(qstr_name), value },

// End the globals table and define the dict and module
#define MP_DUALMODE_MODULE_END() \
    ;

// Define the module globals dict
#define MP_DUALMODE_DEFINE_MODULE(module_name, qstr_name) \
    static MP_DEFINE_CONST_DICT(module_name##_globals, module_name##_globals_table); \
    \
    const mp_obj_module_t module_name##_user_cmodule = { \
        .base = { &mp_type_module }, \
        .globals = (mp_obj_dict_t*)&module_name##_globals, \
    }; \
    \
    MP_REGISTER_MODULE(qstr_name, module_name##_user_cmodule);

// In user C module mode, this does nothing
#define MP_DUALMODE_INIT_MODULE(module_name)

// Type compatibility macros
#define MP_DUALMODE_ROM_QSTR(q) MP_ROM_QSTR(q)
#define MP_DUALMODE_ROM_INT(i) MP_ROM_INT(i)
#define MP_DUALMODE_ROM_PTR(p) MP_ROM_PTR(p)

#endif // MICROPY_ENABLE_DYNRUNTIME

// ========== Common Macros ==========

// Helper to ensure QSTRs are defined in both modes
#ifdef MICROPY_ENABLE_DYNRUNTIME
// For native modules, create references to ensure QSTRs are included
#define MP_DUALMODE_QSTR_DEF(name) \
    static void _qstr_ref_##name(void) { (void)MP_QSTR_##name; }
#else
// For user C modules, QSTRs are handled by the build system
#define MP_DUALMODE_QSTR_DEF(name)
#endif

// Convenience macro to define a module with standard structure
#define MP_DUALMODE_MODULE_BEGIN(module_name, qstr_name) \
    MP_DUALMODE_QSTR_DEF(qstr_name) \
    MP_DUALMODE_MODULE_GLOBALS_TABLE(module_name) = { \
        MP_DUALMODE_MODULE_ATTR(MP_QSTR___name__, MP_DUALMODE_ROM_QSTR(MP_QSTR_##qstr_name))

// Close the module definition
#define MP_DUALMODE_MODULE_COMPLETE(module_name, qstr_name) \
    MP_DUALMODE_MODULE_END() \
    MP_DUALMODE_INIT_MODULE(module_name)

// For user C modules only, finalize the module
#ifndef MICROPY_ENABLE_DYNRUNTIME
#define MP_DUALMODE_MODULE_FINALIZE(module_name, qstr_name) \
    MP_DUALMODE_DEFINE_MODULE(module_name, qstr_name)
#else
#define MP_DUALMODE_MODULE_FINALIZE(module_name, qstr_name)
#endif

#endif // MICROPY_INCLUDED_MP_DUALMODE_H