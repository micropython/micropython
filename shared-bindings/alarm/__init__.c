#include "shared-bindings/alarm/__init__.h"

//| """alarm module
//|
//| The `alarm` module implements deep sleep."""

STATIC mp_obj_t alarm_disable_all(void) {
    common_hal_alarm_disable_all();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_disable_all_obj, alarm_disable_all);

STATIC mp_obj_t alarm_get_wake_alarm(void) {
    return common_hal_alarm_get_wake_alarm();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_get_wake_alarm_obj, alarm_get_wake_alarm);

STATIC const mp_rom_map_elem_t alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm) },
    { MP_ROM_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&alarm_disable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_wake_alarm), MP_ROM_PTR(&alarm_get_wake_alarm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_module_globals, alarm_module_globals_table);

const mp_obj_module_t alarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_module_globals,
};
