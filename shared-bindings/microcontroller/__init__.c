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

#include "supervisor/shared/translate.h"

//| """Pin references and cpu functionality
//|
//| The `microcontroller` module defines the pins from the perspective of the
//| microcontroller. See :py:mod:`board` for board-specific pin mappings."""
//|
//| from nvm import ByteArray
//| from watchdog import WatchDogTimer
//|

//| cpu: Processor
//| """CPU information and control, such as ``cpu.temperature`` and ``cpu.frequency``
//| (clock frequency).
//| This object is an instance of `microcontroller.Processor`."""
//|

//| cpus: Processor
//| """CPU information and control, such as ``cpus[0].temperature`` and ``cpus[1].frequency``
//| (clock frequency) on chips with more than 1 cpu. The index selects which cpu.
//| This object is an instance of `microcontroller.Processor`."""
//|

//| def delay_us(delay: int) -> None:
//|     """Dedicated delay method used for very short delays. **Do not** do long delays
//|     because this stops all other functions from completing. Think of this as an empty
//|     ``while`` loop that runs for the specified ``(delay)`` time. If you have other
//|     code or peripherals (e.g audio recording) that require specific timing or
//|     processing while you are waiting, explore a different avenue such as using
//|     `time.sleep()`."""
//|     ...
//|
STATIC mp_obj_t mcu_delay_us(mp_obj_t delay_obj) {
    uint32_t delay = mp_obj_get_int(delay_obj);

    common_hal_mcu_delay_us(delay);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mcu_delay_us_obj, mcu_delay_us);

//| def disable_interrupts() -> None:
//|     """Disable all interrupts. Be very careful, this can stall everything."""
//|     ...
//|
STATIC mp_obj_t mcu_disable_interrupts(void) {
    common_hal_mcu_disable_interrupts();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mcu_disable_interrupts_obj, mcu_disable_interrupts);

//| def enable_interrupts() -> None:
//|     """Enable the interrupts that were enabled at the last disable."""
//|     ...
//|
STATIC mp_obj_t mcu_enable_interrupts(void) {
    common_hal_mcu_enable_interrupts();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mcu_enable_interrupts_obj, mcu_enable_interrupts);

//| def on_next_reset(run_mode: microcontroller.RunMode) -> None:
//|     """Configure the run mode used the next time the microcontroller is reset but
//|     not powered down.
//|
//|     :param ~microcontroller.RunMode run_mode: The next run mode"""
//|     ...
//|
STATIC mp_obj_t mcu_on_next_reset(mp_obj_t run_mode_obj) {
    mcu_runmode_t run_mode;
    if (run_mode_obj == &mcu_runmode_normal_obj) {
        run_mode = RUNMODE_NORMAL;
    } else if (run_mode_obj == &mcu_runmode_safe_mode_obj) {
        run_mode = RUNMODE_SAFE_MODE;
    } else if (run_mode_obj == &mcu_runmode_bootloader_obj) {
        run_mode = RUNMODE_BOOTLOADER;
    } else {
        mp_raise_ValueError(translate("Invalid run mode."));
    }

    common_hal_mcu_on_next_reset(run_mode);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mcu_on_next_reset_obj, mcu_on_next_reset);

//| def reset() -> None:
//|     """Reset the microcontroller. After reset, the microcontroller will enter the
//|     run mode last set by `on_next_reset`.
//|
//|     .. warning:: This may result in file system corruption when connected to a
//|       host computer. Be very careful when calling this! Make sure the device
//|       "Safely removed" on Windows or "ejected" on Mac OSX and Linux."""
//|     ...
//|
STATIC mp_obj_t mcu_reset(void) {
    common_hal_mcu_reset();
    // We won't actually get here because we're resetting.
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mcu_reset_obj, mcu_reset);

//| nvm: Optional[ByteArray]
//| """Available non-volatile memory.
//| This object is the sole instance of `nvm.ByteArray` when available or ``None`` otherwise.
//|
//| :type: nvm.ByteArray or None"""
//|

//| watchdog: Optional[WatchDogTimer]
//| """Available watchdog timer.
//| This object is the sole instance of `watchdog.WatchDogTimer` when available or ``None`` otherwise."""
//|

const mp_obj_module_t mcu_pin_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mcu_pin_globals,
};

STATIC const mp_rom_map_elem_t mcu_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_microcontroller) },
    { MP_ROM_QSTR(MP_QSTR_cpu),  MP_ROM_PTR(&common_hal_mcu_processor_obj) },
    #if CIRCUITPY_PROCESSOR_COUNT > 1
    { MP_ROM_QSTR(MP_QSTR_cpus),  MP_ROM_PTR(&common_hal_multi_processor_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_delay_us), MP_ROM_PTR(&mcu_delay_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_interrupts), MP_ROM_PTR(&mcu_disable_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupts), MP_ROM_PTR(&mcu_enable_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_next_reset), MP_ROM_PTR(&mcu_on_next_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&mcu_reset_obj) },
    #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
    { MP_ROM_QSTR(MP_QSTR_nvm),  MP_ROM_PTR(&common_hal_mcu_nvm_obj) },
    #else
    { MP_ROM_QSTR(MP_QSTR_nvm),  MP_ROM_NONE },
    #endif
    #if CIRCUITPY_WATCHDOG
    { MP_ROM_QSTR(MP_QSTR_watchdog),  MP_ROM_PTR(&common_hal_mcu_watchdogtimer_obj) },
    #else
    { MP_ROM_QSTR(MP_QSTR_watchdog),  MP_ROM_NONE },
    #endif
    { MP_ROM_QSTR(MP_QSTR_ResetReason),  MP_ROM_PTR(&mcu_reset_reason_type) },
    { MP_ROM_QSTR(MP_QSTR_RunMode),  MP_ROM_PTR(&mcu_runmode_type) },
    { MP_ROM_QSTR(MP_QSTR_Pin),  MP_ROM_PTR(&mcu_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_pin),  MP_ROM_PTR(&mcu_pin_module) },
    { MP_ROM_QSTR(MP_QSTR_Processor),   MP_ROM_PTR(&mcu_processor_type) },

};

STATIC MP_DEFINE_CONST_DICT(mcu_module_globals, mcu_module_globals_table);

const mp_obj_module_t microcontroller_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mcu_module_globals,
};
