#include "py/obj.h"
#include "shared-bindings/time_alarm/__init__.h"

//| Set Timer Wakeup 
//|
STATIC mp_obj_t time_alarm_duration(mp_obj_t seconds_o) { 
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
    common_hal_time_alarm_duration(msecs);

    time_alarm_obj_t *self = m_new_obj(time_alarm_obj_t);
    self->base.type = &time_alarm_type;

    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_alarm_duration_obj, time_alarm_duration);

//| Disable Timer Wakeup 
//|
STATIC mp_obj_t time_alarm_disable(void) {      
    common_hal_time_alarm_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(time_alarm_disable_obj, time_alarm_disable);

STATIC const mp_rom_map_elem_t time_alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time_alarm) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Duration), MP_ROM_PTR(&time_alarm_duration_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&time_alarm_disable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(time_alarm_module_globals, time_alarm_module_globals_table);

const mp_obj_module_t time_alarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&time_alarm_module_globals,
};

const mp_obj_type_t time_alarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_timeAlarm,
};
