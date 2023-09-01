/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared-bindings/util.h"

#include "bindings/espulp/__init__.h"
#include "bindings/espulp/ULP.h"
#include "bindings/espulp/ULPAlarm.h"
#include "bindings/espulp/Architecture.h"

#include "py/runtime.h"

//| """ESP Ultra Low Power Processor Module
//|
//| The `espulp` module adds ability to load and run
//| programs on the ESP32-Sx's ultra-low-power RISC-V processor.
//|
//| .. code-block:: python
//|
//|     import espulp
//|     import memorymap
//|
//|     shared_mem = memorymap.AddressRange(start=0x50000000, length=1024)
//|     ulp = espulp.ULP()
//|
//|     with open("program.bin", "rb") as f:
//|         program = f.read()
//|
//|     ulp.run(program)
//|     print(shared_mem[0])
//|     # ulp.halt()
//| """
//| ...
//|

//| def get_rtc_gpio_number(pin: microcontroller.Pin) -> Optional[int]:
//|     """Return the RTC GPIO number of the given pin or None if not connected
//|     to RTC GPIO."""
//|     ...
//|

STATIC mp_obj_t espulp_get_rtc_gpio_number(mp_obj_t pin_obj) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin(pin_obj, MP_QSTR_pin);
    mp_int_t number = common_hal_espulp_get_rtc_gpio_number(pin);
    if (number < 0) {
        return mp_const_none;
    }
    return MP_OBJ_NEW_SMALL_INT(number);
}
MP_DEFINE_CONST_FUN_OBJ_1(espulp_get_rtc_gpio_number_obj, espulp_get_rtc_gpio_number);

STATIC const mp_rom_map_elem_t espulp_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espulp) },

    // module functions
    { MP_ROM_QSTR(MP_QSTR_get_rtc_gpio_number), MP_OBJ_FROM_PTR(&espulp_get_rtc_gpio_number_obj) },

    // module classes
    { MP_ROM_QSTR(MP_QSTR_ULP), MP_OBJ_FROM_PTR(&espulp_ulp_type) },
    { MP_ROM_QSTR(MP_QSTR_ULPAlarm), MP_OBJ_FROM_PTR(&espulp_ulpalarm_type) },
    { MP_ROM_QSTR(MP_QSTR_Architecture), MP_ROM_PTR(&espulp_architecture_type) },
};
STATIC MP_DEFINE_CONST_DICT(espulp_module_globals, espulp_module_globals_table);

const mp_obj_module_t espulp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espulp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_espulp, espulp_module);
