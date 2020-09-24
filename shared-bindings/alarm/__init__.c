#include "shared-bindings/alarm/__init__.h"

//| def getWakeAlarm() -> None:
//|     """This returns the alarm that triggered wakeup, 
//|     also returns alarm specific parameters`.
//|
STATIC mp_obj_t alarm_get_wake_alarm(void) {
    return common_hal_alarm_get_wake_alarm();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_get_wake_alarm_obj, alarm_get_wake_alarm);

//| def getSleepTime() -> None:
//|     """This returns the period of time in ms, 
//|     in which the board was in deep sleep`.
//|
STATIC mp_obj_t alarm_get_sleep_time(void) {
    return mp_obj_new_int(common_hal_alarm_get_sleep_time());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_get_sleep_time_obj, alarm_get_sleep_time);

STATIC const mp_rom_map_elem_t alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm) },
    { MP_ROM_QSTR(MP_QSTR_getSleepTime), MP_ROM_PTR(&alarm_get_sleep_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_getWakeAlarm), MP_ROM_PTR(&alarm_get_wake_alarm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_module_globals, alarm_module_globals_table);

const mp_obj_module_t alarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_module_globals,
};
