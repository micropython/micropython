#include "py/obj.h"
#include "shared-bindings/alarm_touch/__init__.h"

STATIC mp_obj_t alarm_touch_disable(void) {
    common_hal_alarm_touch_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_touch_disable_obj, alarm_touch_disable);

STATIC const mp_rom_map_elem_t alarm_touch_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm_touch) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&alarm_touch_disable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(alarm_touch_module_globals, alarm_touch_module_globals_table);

const mp_obj_module_t alarm_touch_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_touch_module_globals,
};

const mp_obj_type_t alarm_touch_type = {
    { &mp_type_type },
    .name = MP_QSTR_touchAlarm,
};
