#include "py/obj.h"

#include "shared-bindings/alarm_io/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

//| Set Pin Wakeup 
//|
STATIC mp_obj_t alarm_io_pin_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {  
    enum { ARG_level, ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_level, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_pull, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_bool = false} },
    }; 

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);  

    mcu_pin_obj_t *pin = validate_obj_is_pin(pos_args[0]);
    alarm_io_obj_t *self = m_new_obj(alarm_io_obj_t);

    self->base.type = &alarm_io_type;
    self->gpio = pin->number;
    self->level = args[ARG_level].u_int;
    self->pull = args[ARG_pull].u_bool;
    
    return common_hal_alarm_io_pin_state(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(alarm_io_pin_state_obj, 1, alarm_io_pin_state);


//| Disable Pin Wakeup 
//|
STATIC mp_obj_t alarm_io_disable(void) {      
    common_hal_alarm_io_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_io_disable_obj, alarm_io_disable);

STATIC const mp_rom_map_elem_t alarm_io_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm_io) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PinState), MP_ROM_PTR(&alarm_io_pin_state_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&alarm_io_disable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(alarm_io_module_globals, alarm_io_module_globals_table);

const mp_obj_module_t alarm_io_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_io_module_globals,
};

const mp_obj_type_t alarm_io_type = {
    { &mp_type_type },
    .name = MP_QSTR_ioAlarm,
};
