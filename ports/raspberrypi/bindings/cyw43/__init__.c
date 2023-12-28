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

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#include "lib/cyw43-driver/src/cyw43.h"

static int power_management_value = PM_DISABLED;

void cyw43_enter_deep_sleep(void) {
#define WL_REG_ON 23
    gpio_set_dir(WL_REG_ON, GPIO_OUT);
    gpio_put(WL_REG_ON, false);
}

void bindings_cyw43_wifi_enforce_pm(void) {
    cyw43_wifi_pm(&cyw43_state, power_management_value);
}

//| class CywPin:
//|     """A class that represents a GPIO pin attached to the wifi chip.
//|
//|     Cannot be constructed at runtime, but may be the type of a pin object
//|     in :py:mod:`board`. A `CywPin` can be used as a DigitalInOut, but not with other
//|     peripherals such as `PWMOut`."""
//|
MP_DEFINE_CONST_OBJ_TYPE(
    cyw43_pin_type,
    MP_QSTR_CywPin,
    MP_TYPE_FLAG_NONE,
    print, shared_bindings_microcontroller_pin_print
    );

//| PM_STANDARD: int
//| """The standard power management mode"""
//| PM_AGGRESSIVE: int
//| """Aggressive power management mode for optimal power usage at the cost of performance"""
//| PM_PERFORMANCE: int
//| """Performance power management mode where more power is used to increase performance"""
//| PM_DISABLED: int
//| """Disable power management and always use highest power mode. CircuitPython sets this value at reset time, because it provides the best connectivity reliability."""
//|
//| def set_power_management(value: int) -> None:
//|     """Set the power management register
//|
//|     For transmitter power, see ``wifi.Radio.txpower``.
//|     This controls software power saving features inside the cyw43 chip.
//|     it does not control transmitter power.
//|
//|     The value is interpreted as a 24-bit hexadecimal number of the form
//|     ``0x00adbrrm``.
//|
//|     The low 4 bits, ``m``, are the power management mode:
//|      * 0: disabled
//|      * 1: aggressive power saving which reduces wifi throughput
//|      * 2: Power saving with high throughput
//|
//|     The next 8 bits, ``r``, specify "the maximum time to wait before going back to sleep" for power management mode 2. The units of ``r`` are 10ms.
//|
//|     The next 4 bits, ``b``, are the "wake period is measured in beacon periods".
//|
//|     The next 4 bits, ``d``, specify the "wake interval measured in DTIMs. If this is set to 0, the wake interval is measured in beacon periods".
//|
//|     The top 4 bits, ``a``, specifies the "wake interval sent to the access point"
//|
//|     Several ``PM_`` constants gathered from various sources are included
//|     in this module.  According to Raspberry Pi documentation, the value 0xa11140
//|     (called `cyw43.PM_DISABLED` here) increases responsiveness at the cost of higher power
//|     usage.
//|     """
//|
STATIC mp_obj_t cyw43_set_power_management(const mp_obj_t value_in) {
    mp_int_t value = mp_obj_get_int(value_in);
    power_management_value = value;
    bindings_cyw43_wifi_enforce_pm();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cyw43_set_power_management_obj, cyw43_set_power_management);

//| def get_power_management() -> int:
//|     """Retrieve the power management register"""
//|
STATIC mp_obj_t cyw43_get_power_management() {
    return mp_obj_new_int(power_management_value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cyw43_get_power_management_obj, cyw43_get_power_management);

const mcu_pin_obj_t *validate_obj_is_pin_including_cyw43(mp_obj_t obj, qstr arg_name) {
    if (!mp_obj_is_type(obj, &mcu_pin_type) && !mp_obj_is_type(obj, &cyw43_pin_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), arg_name, mcu_pin_type.name, cyw43_pin_type.name, mp_obj_get_type(obj)->name);
    }
    return MP_OBJ_TO_PTR(obj);
}

const mcu_pin_obj_t *validate_obj_is_free_pin_or_gpio29(mp_obj_t obj, qstr arg_name) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin(obj, arg_name);
    if (obj != &pin_GPIO29) {
        assert_pin_free(pin);
    }
    return pin;
}

const mcu_pin_obj_t *validate_obj_is_free_pin_including_cyw43(mp_obj_t obj, qstr arg_name) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin_including_cyw43(obj, arg_name);
    assert_pin_free(pin);
    return pin;
}

STATIC const mp_rom_map_elem_t cyw43_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cyw43) },
    { MP_ROM_QSTR(MP_QSTR_CywPin), MP_ROM_PTR(&cyw43_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_set_power_management), &cyw43_set_power_management_obj },
    { MP_ROM_QSTR(MP_QSTR_get_power_management), &cyw43_get_power_management_obj },
    { MP_ROM_QSTR(MP_QSTR_PM_STANDARD), MP_ROM_INT(PM_STANDARD) },
    { MP_ROM_QSTR(MP_QSTR_PM_AGGRESSIVE), MP_ROM_INT(PM_AGGRESSIVE) },
    { MP_ROM_QSTR(MP_QSTR_PM_PERFORMANCE), MP_ROM_INT(PM_PERFORMANCE) },
    { MP_ROM_QSTR(MP_QSTR_PM_DISABLED), MP_ROM_INT(PM_DISABLED) },
};

STATIC MP_DEFINE_CONST_DICT(cyw43_module_globals, cyw43_module_globals_table);

const mp_obj_module_t cyw43_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cyw43_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cyw43, cyw43_module);
