#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "modmachine_pin.h"
#include "samdneopixel.h"

extern const mp_obj_type_t pin_type;

STATIC mp_obj_t neopixel_write_neopixel_write_(mp_obj_t pin_obj, mp_obj_t buf, mp_obj_t is800k) {
    // Convert parameters into expected types.
    const pin_obj_t *pin = pin_find(pin_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    // Call platform's neopixel write function with provided buffer and options.
    samd_neopixel_write(pin->pin, (uint8_t*)bufinfo.buf, bufinfo.len,
        mp_obj_is_true(is800k));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(neopixel_write_neopixel_write_obj, neopixel_write_neopixel_write_);

STATIC const mp_rom_map_elem_t neopixel_write_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write), (mp_obj_t)&neopixel_write_neopixel_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(neopixel_write_module_globals, neopixel_write_module_globals_table);

const mp_obj_module_t neopixel_write_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&neopixel_write_module_globals,
};
