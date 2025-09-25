/*
 * Example of a dual-mode module that works as both:
 * - User C module (compiled into firmware)
 * - Native module (dynamic .mpy file)
 * 
 * This demonstrates how to write modules that work in both contexts.
 */

#include "mp_dualmode.h"

// Include time functionality
#ifndef MICROPY_ENABLE_DYNRUNTIME
#include "py/mphal.h"  // For user C modules
#else
// Native modules get mp_hal_ticks_ms via dynruntime
#endif

// Define QSTRs we'll use (ensures they're included in native module)
MP_DUALMODE_QSTR_DEF(dualmode_example)
MP_DUALMODE_QSTR_DEF(add)
MP_DUALMODE_QSTR_DEF(multiply)
MP_DUALMODE_QSTR_DEF(get_info)
MP_DUALMODE_QSTR_DEF(Timer)
MP_DUALMODE_QSTR_DEF(elapsed)
MP_DUALMODE_QSTR_DEF(reset)
MP_DUALMODE_QSTR_DEF(VERSION)
MP_DUALMODE_QSTR_DEF(MODE)

// Simple function: add two integers
static mp_obj_t dualmode_add(mp_obj_t a_obj, mp_obj_t b_obj) {
    mp_int_t a = mp_obj_get_int(a_obj);
    mp_int_t b = mp_obj_get_int(b_obj);
    return mp_obj_new_int(a + b);
}
static MP_DEFINE_CONST_FUN_OBJ_2(dualmode_add_obj, dualmode_add);

// Function with variable arguments: multiply all arguments
static mp_obj_t dualmode_multiply(size_t n_args, const mp_obj_t *args) {
    mp_int_t result = 1;
    for (size_t i = 0; i < n_args; i++) {
        result *= mp_obj_get_int(args[i]);
    }
    return mp_obj_new_int(result);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(dualmode_multiply_obj, 0, dualmode_multiply);

// Function that returns module info
static mp_obj_t dualmode_get_info(void) {
    const char *mode = 
#ifdef MICROPY_ENABLE_DYNRUNTIME
        "native module (.mpy)";
#else
        "user C module (firmware)";
#endif
    
    // Create a dictionary with module information
    mp_obj_dict_t *dict = mp_obj_new_dict(2);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_version), MP_OBJ_NEW_SMALL_INT(100));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_mode), mp_obj_new_str(mode, strlen(mode)));
    return MP_OBJ_FROM_PTR(dict);
}
static MP_DEFINE_CONST_FUN_OBJ_0(dualmode_get_info_obj, dualmode_get_info);

// Timer class example
typedef struct _dualmode_Timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t start_time;
} dualmode_Timer_obj_t;

// Timer.__init__
static mp_obj_t dualmode_Timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    
    // Allocate timer object
#ifdef MICROPY_ENABLE_DYNRUNTIME
    dualmode_Timer_obj_t *self = m_new_obj(dualmode_Timer_obj_t);
    self->base.type = type;
#else
    dualmode_Timer_obj_t *self = mp_obj_malloc(dualmode_Timer_obj_t, type);
#endif
    
    // Initialize with current time
    self->start_time = mp_hal_ticks_ms();
    
    return MP_OBJ_FROM_PTR(self);
}

// Timer.elapsed()
static mp_obj_t dualmode_Timer_elapsed(mp_obj_t self_in) {
    dualmode_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t elapsed = mp_hal_ticks_ms() - self->start_time;
    return mp_obj_new_int_from_uint(elapsed);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dualmode_Timer_elapsed_obj, dualmode_Timer_elapsed);

// Timer.reset()
static mp_obj_t dualmode_Timer_reset(mp_obj_t self_in) {
    dualmode_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->start_time = mp_hal_ticks_ms();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(dualmode_Timer_reset_obj, dualmode_Timer_reset);

// Timer class locals dict
static const mp_rom_map_elem_t dualmode_Timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_elapsed), MP_ROM_PTR(&dualmode_Timer_elapsed_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&dualmode_Timer_reset_obj) },
};
static MP_DEFINE_CONST_DICT(dualmode_Timer_locals_dict, dualmode_Timer_locals_dict_table);

// Define Timer type
#ifdef MICROPY_ENABLE_DYNRUNTIME
// For native modules, we need to define the type at runtime
static mp_obj_type_t dualmode_Timer_type;

static void init_timer_type(void) {
    dualmode_Timer_type.base.type = &mp_type_type;
    dualmode_Timer_type.name = MP_QSTR_Timer;
    dualmode_Timer_type.flags = MP_TYPE_FLAG_NONE;
    dualmode_Timer_type.make_new = dualmode_Timer_make_new;
    dualmode_Timer_type.locals_dict = (mp_obj_dict_t*)&dualmode_Timer_locals_dict;
}
#else
// For user C modules, use the standard macro
MP_DEFINE_CONST_OBJ_TYPE(
    dualmode_Timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, dualmode_Timer_make_new,
    locals_dict, &dualmode_Timer_locals_dict
);
#endif

// Module globals table using dual-mode macros
MP_DUALMODE_MODULE_BEGIN(dualmode_example, dualmode_example)
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_add, MP_DUALMODE_ROM_PTR(&dualmode_add_obj))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_multiply, MP_DUALMODE_ROM_PTR(&dualmode_multiply_obj))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_get_info, MP_DUALMODE_ROM_PTR(&dualmode_get_info_obj))
#ifdef MICROPY_ENABLE_DYNRUNTIME
    // Native modules need runtime initialization
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_Timer, MP_ROM_PTR(&dualmode_Timer_type))
#else
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_Timer, MP_DUALMODE_ROM_PTR(&dualmode_Timer_type))
#endif
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_VERSION, MP_DUALMODE_ROM_INT(100))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_MODE, 
#ifdef MICROPY_ENABLE_DYNRUNTIME
        MP_OBJ_NEW_QSTR(MP_QSTR_native)
#else
        MP_DUALMODE_ROM_QSTR(MP_QSTR_firmware)
#endif
    )
MP_DUALMODE_MODULE_COMPLETE(dualmode_example, dualmode_example)

// Native module specific initialization
#ifdef MICROPY_ENABLE_DYNRUNTIME
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Initialize the Timer type for native module
    init_timer_type();
    
    // Register module globals
    MP_DUALMODE_REGISTER_MODULE_GLOBALS(dualmode_example);
    
    MP_DYNRUNTIME_INIT_EXIT
}
#endif

// User C module finalization
MP_DUALMODE_MODULE_FINALIZE(dualmode_example, MP_QSTR_dualmode_example)