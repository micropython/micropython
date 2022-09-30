/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
 * Copyright (c) 2016 Scott Shawcroft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/cyw43/__init__.h"

//| class CywPin:
//|     """A class that represents a GPIO pin attached to the wifi chip.
//|
//|     Cannot be constructed at runtime, but may be the type of a pin object
//|     in :py:mod:`board`. A `CywPin` can be used as a DigitalInOut, but not with other
//|     peripherals such as `PWMOut`."""
//|
const mp_obj_type_t cyw43_pin_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_CywPin,
    .print = shared_bindings_microcontroller_pin_print,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = mp_generic_unary_op,
        )
};

//| def set_power_management(value: int) -> None:
//|     """Set the power management register
//|
//|     According to Raspberry Pi documentation, the value 0xa11140
//|     increases responsiveness at the cost of higher power usage.
//|
//|     Besides this value, there appears to be no other public documentation
//|     of the values that can be used.
//|     """
//|
STATIC mp_obj_t cyw43_set_power_management(const mp_obj_t value_in) {
    mp_int_t value = mp_obj_get_int(value_in);
    cyw43_wifi_pm(&cyw43_state, value);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cyw43_set_power_management_obj, cyw43_set_power_management);

const mcu_pin_obj_t *validate_obj_is_pin_including_cyw43(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &mcu_pin_type) && !mp_obj_is_type(obj, &cyw43_pin_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q or %q"), mcu_pin_type.name, cyw43_pin_type.name);
    }
    return MP_OBJ_TO_PTR(obj);
}

const mcu_pin_obj_t *validate_obj_is_free_pin_including_cyw43(mp_obj_t obj) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin_including_cyw43(obj);
    assert_pin_free(pin);
    return pin;
}

STATIC const mp_rom_map_elem_t cyw43_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cyw43) },
    { MP_ROM_QSTR(MP_QSTR_CywPin), MP_ROM_QSTR(MP_QSTR_CywPin) },
    { MP_ROM_QSTR(MP_QSTR_set_power_management), &cyw43_set_power_management_obj },
};

STATIC MP_DEFINE_CONST_DICT(cyw43_module_globals, cyw43_module_globals_table);

const mp_obj_module_t cyw43_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cyw43_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cyw43, cyw43_module, CIRCUITPY_CYW43);
