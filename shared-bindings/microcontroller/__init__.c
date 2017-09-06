/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

// Microcontroller contains pin references and microcontroller specific control
// functions.

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"


#include "py/runtime.h"

//| :mod:`microcontroller` --- Pin references and cpu functionality
//| ================================================================
//|
//| .. module:: microcontroller
//|   :synopsis: Pin references and core functionality
//|   :platform: SAMD21, ESP8266
//|
//| The `microcontroller` module defines the pins from the perspective of the
//| microcontroller. See `board` for board-specific pin mappings.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     Pin
//|     Processor
//|

//| .. attribute:: cpu
//|
//|   CPU information and control, such as `cpu.temperature` and `cpu.frequency`
//|   (clock frequency).
//|   This object is the sole instance of `microcontroller.Processor`.
//|

//| .. method:: delay_us(delay)
//|
//|   Dedicated delay method used for very short delays. **Do not** do long delays
//|   because it will stall any concurrent code.
//|
STATIC mp_obj_t mcu_delay_us(mp_obj_t delay_obj) {
    uint32_t delay = mp_obj_get_int(delay_obj);

    common_hal_mcu_delay_us(delay);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mcu_delay_us_obj, mcu_delay_us);

//| .. method:: disable_interrupts()
//|
//|   Disable all interrupts. Be very careful, this can stall everything.
//|
STATIC mp_obj_t mcu_disable_interrupts(void) {
    common_hal_mcu_disable_interrupts();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mcu_disable_interrupts_obj, mcu_disable_interrupts);

//| .. method:: enable_interrupts()
//|
//|   Enable the interrupts that were enabled at the last disable.
//|
STATIC mp_obj_t mcu_enable_interrupts(void) {
    common_hal_mcu_enable_interrupts();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mcu_enable_interrupts_obj, mcu_enable_interrupts);

//| .. attribute:: nvm
//|
//|   Available non-volatile memory.
//|   This object is the sole instance of `nvm.ByteArray` when available or ``None`` otherwise.
//|

//| :mod:`microcontroller.pin` --- Microcontroller pin names
//| --------------------------------------------------------
//|
//| .. module:: microcontroller.pin
//|   :synopsis: Microcontroller pin names
//|   :platform: SAMD21
//|
//| References to pins as named by the microcontroller
//|
const mp_obj_module_t mcu_pin_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mcu_pin_globals,
};

STATIC const mp_rom_map_elem_t mcu_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_microcontroller) },
    { MP_ROM_QSTR(MP_QSTR_cpu),  MP_ROM_PTR(&common_hal_mcu_processor_obj) },
    { MP_ROM_QSTR(MP_QSTR_delay_us), MP_ROM_PTR(&mcu_delay_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_interrupts), MP_ROM_PTR(&mcu_disable_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupts), MP_ROM_PTR(&mcu_enable_interrupts_obj) },
    #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
    { MP_ROM_QSTR(MP_QSTR_nvm),  MP_ROM_PTR(&common_hal_mcu_nvm_obj) },
    #else
    { MP_ROM_QSTR(MP_QSTR_nvm),  MP_ROM_PTR(&mp_const_none_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_Pin),  MP_ROM_PTR(&mcu_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_pin),  MP_ROM_PTR(&mcu_pin_module) },
    { MP_ROM_QSTR(MP_QSTR_Processor),   MP_ROM_PTR(&mcu_processor_type) },

};

STATIC MP_DEFINE_CONST_DICT(mcu_module_globals, mcu_module_globals_table);

const mp_obj_module_t microcontroller_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mcu_module_globals,
};
