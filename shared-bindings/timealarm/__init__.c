#include "py/obj.h"
#include "shared-bindings/timealarm/__init__.h"

//| Set Timer Wakeup 
//|
STATIC mp_obj_t timealarm_duration(mp_obj_t seconds_o) { 
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_float_t seconds = mp_obj_get_float(seconds_o);
    mp_float_t msecs = 1000.0f * seconds + 0.5f;
    #else
    mp_int_t seconds = mp_obj_get_int(seconds_o);
    mp_int_t msecs = 1000 * seconds;
    #endif
    if (seconds < 0) {
        mp_raise_ValueError(translate("sleep length must be non-negative"));
    }
    common_hal_timealarm_duration(msecs);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(timealarm_duration_obj, timealarm_duration);

STATIC const mp_rom_map_elem_t timealarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_timealarm) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_duration), MP_ROM_PTR(&timealarm_duration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(timealarm_module_globals, timealarm_module_globals_table);

const mp_obj_module_t timealarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&timealarm_module_globals,
};
