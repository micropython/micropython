/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Processor.h"

#include <math.h>
#include <stdint.h>

#include "py/objproperty.h"

#include "py/runtime.h"

//| class Processor:
//|     """Microcontroller CPU information and control
//|
//|     Usage::
//|
//|        import microcontroller
//|        print(microcontroller.cpu.frequency)
//|        print(microcontroller.cpu.temperature)
//|
//|        Note that on chips with more than one cpu (such as the RP2040)
//|        microcontroller.cpu will return the value for CPU 0.
//|        To get values from other CPUs use microcontroller.cpus indexed by
//|        the number of the desired cpu. i.e.
//|
//|        print(microcontroller.cpus[0].temperature)
//|        print(microcontroller.cpus[1].frequency)"""
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `microcontroller.Processor`.
//|         Use `microcontroller.cpu` to access the sole instance available."""
//|         ...
//|

//|     frequency: int
//|     """The CPU operating frequency in Hertz. (read-only)"""
//|
STATIC mp_obj_t mcu_processor_get_frequency(mp_obj_t self) {
    return mp_obj_new_int_from_uint(common_hal_mcu_processor_get_frequency());
}

MP_DEFINE_CONST_FUN_OBJ_1(mcu_processor_get_frequency_obj, mcu_processor_get_frequency);

const mp_obj_property_t mcu_processor_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mcu_processor_get_frequency_obj,  // getter
              MP_ROM_NONE,            // no setter
              MP_ROM_NONE,            // no deleter
    },
};

//|     reset_reason: microcontroller.ResetReason
//|     """The reason the microcontroller started up from reset state."""
//|
STATIC mp_obj_t mcu_processor_get_reset_reason(mp_obj_t self) {
    return cp_enum_find(&mcu_reset_reason_type, common_hal_mcu_processor_get_reset_reason());
}

MP_DEFINE_CONST_FUN_OBJ_1(mcu_processor_get_reset_reason_obj, mcu_processor_get_reset_reason);

const mp_obj_property_t mcu_processor_reset_reason_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mcu_processor_get_reset_reason_obj,  // getter
              MP_ROM_NONE,            // no setter
              MP_ROM_NONE,            // no deleter
    },
};

//|     temperature: Optional[float]
//|     """The on-chip temperature, in Celsius, as a float. (read-only)
//|
//|     Is `None` if the temperature is not available."""
//|
STATIC mp_obj_t mcu_processor_get_temperature(mp_obj_t self) {
    float temperature = common_hal_mcu_processor_get_temperature();
    return isnan(temperature) ? mp_const_none : mp_obj_new_float(temperature);
}

MP_DEFINE_CONST_FUN_OBJ_1(mcu_processor_get_temperature_obj, mcu_processor_get_temperature);

const mp_obj_property_t mcu_processor_temperature_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mcu_processor_get_temperature_obj,  // getter
              MP_ROM_NONE,            // no setter
              MP_ROM_NONE,            // no deleter
    },
};

//|     uid: bytearray
//|     """The unique id (aka serial number) of the chip as a `bytearray`. (read-only)"""
//|
STATIC mp_obj_t mcu_processor_get_uid(mp_obj_t self) {
    uint8_t raw_id[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH];
    common_hal_mcu_processor_get_uid(raw_id);
    return mp_obj_new_bytearray(sizeof(raw_id), raw_id);
}

MP_DEFINE_CONST_FUN_OBJ_1(mcu_processor_get_uid_obj, mcu_processor_get_uid);

const mp_obj_property_t mcu_processor_uid_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mcu_processor_get_uid_obj,  // getter
              MP_ROM_NONE,      // no setter
              MP_ROM_NONE,      // no deleter
    },
};

//|     voltage: Optional[float]
//|     """The input voltage to the microcontroller, as a float. (read-only)
//|
//|     Is `None` if the voltage is not available."""
//|
STATIC mp_obj_t mcu_processor_get_voltage(mp_obj_t self) {
    float voltage = common_hal_mcu_processor_get_voltage();
    return isnan(voltage) ? mp_const_none : mp_obj_new_float(voltage);
}

MP_DEFINE_CONST_FUN_OBJ_1(mcu_processor_get_voltage_obj, mcu_processor_get_voltage);

const mp_obj_property_t mcu_processor_voltage_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mcu_processor_get_voltage_obj,  // getter
              MP_ROM_NONE,            // no setter
              MP_ROM_NONE,            // no deleter
    },
};

STATIC const mp_rom_map_elem_t mcu_processor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&mcu_processor_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_reason), MP_ROM_PTR(&mcu_processor_reset_reason_obj) },
    { MP_ROM_QSTR(MP_QSTR_temperature), MP_ROM_PTR(&mcu_processor_temperature_obj) },
    { MP_ROM_QSTR(MP_QSTR_uid), MP_ROM_PTR(&mcu_processor_uid_obj) },
    { MP_ROM_QSTR(MP_QSTR_voltage), MP_ROM_PTR(&mcu_processor_voltage_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mcu_processor_locals_dict, mcu_processor_locals_dict_table);

const mp_obj_type_t mcu_processor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Processor,
    .locals_dict = (mp_obj_t)&mcu_processor_locals_dict,
};
