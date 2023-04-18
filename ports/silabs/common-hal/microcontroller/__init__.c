/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"
#include "em_chip.h"
#include "em_core.h"
#include "sl_udelay.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    sl_udelay_wait(delay);
}

void common_hal_mcu_disable_interrupts(void) {
    CORE_CriticalDisableIrq();
}

void common_hal_mcu_enable_interrupts(void) {
    CORE_CriticalEnableIrq();
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush(); // TODO: implement as part of flash improvements
    CHIP_Reset();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = NVM_BYTEARRAY_BUFFER_SIZE
};

#endif

#if CIRCUITPY_WATCHDOG
// The singleton watchdog.WatchDogTimer object.
watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj = {
    .base = {
        .type = &watchdog_watchdogtimer_type,
    },
    .timeout = 0.0f,
    .mode = WATCHDOGMODE_NONE,
};
#endif

// This maps MCU pin names to pin objects.
const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {

    #ifdef GPIO_PA0_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA0), MP_ROM_PTR(&pin_PA0) },
    #endif

    #ifdef GPIO_PA1_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA1), MP_ROM_PTR(&pin_PA1) },
    #endif

    #ifdef GPIO_PA2_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA2), MP_ROM_PTR(&pin_PA2) },
    #endif

    #ifdef GPIO_PA3_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA3), MP_ROM_PTR(&pin_PA3) },
    #endif

    #ifdef GPIO_PA4_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA4), MP_ROM_PTR(&pin_PA4) },
    #endif

    #ifdef GPIO_PA5_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA5), MP_ROM_PTR(&pin_PA5) },
    #endif
    #ifdef GPIO_PA6_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA6), MP_ROM_PTR(&pin_PA6) },
    #endif

    #ifdef GPIO_PA7_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA7), MP_ROM_PTR(&pin_PA7) },
    #endif

    #ifdef GPIO_PA8_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PA8), MP_ROM_PTR(&pin_PA8) },
    #endif

    #ifdef GPIO_PB0_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB0), MP_ROM_PTR(&pin_PB0) },
    #endif

    #ifdef GPIO_PB1_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB1), MP_ROM_PTR(&pin_PB1) },
    #endif

    #ifdef GPIO_PB2_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB2), MP_ROM_PTR(&pin_PB2) },
    #endif

    #ifdef GPIO_PB3_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB3), MP_ROM_PTR(&pin_PB3) },
    #endif

    #ifdef GPIO_PB4_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB4), MP_ROM_PTR(&pin_PB4) },
    #endif

    #ifdef GPIO_PB5_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PB5), MP_ROM_PTR(&pin_PB5) },
    #endif

    #ifdef GPIO_PC0_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC0), MP_ROM_PTR(&pin_PC0) },
    #endif

    #ifdef GPIO_PC1_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC1), MP_ROM_PTR(&pin_PC1) },
    #endif

    #ifdef GPIO_PC2_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC2), MP_ROM_PTR(&pin_PC2) },
    #endif

    #ifdef GPIO_PC3_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC3), MP_ROM_PTR(&pin_PC3) },
    #endif

    #ifdef GPIO_PC4_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC4), MP_ROM_PTR(&pin_PC4) },
    #endif

    #ifdef GPIO_PC5_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC5), MP_ROM_PTR(&pin_PC5) },
    #endif

    #ifdef GPIO_PC6_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC6), MP_ROM_PTR(&pin_PC6) },
    #endif

    #ifdef GPIO_PC7_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC7), MP_ROM_PTR(&pin_PC7) },
    #endif
    #ifdef GPIO_PC8_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC8), MP_ROM_PTR(&pin_PC8) },
    #endif

    #ifdef GPIO_PC9_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PC9), MP_ROM_PTR(&pin_PC9) },
    #endif

    #ifdef GPIO_PD0_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD0), MP_ROM_PTR(&pin_PD0) },
    #endif

    #ifdef GPIO_PD1_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD1), MP_ROM_PTR(&pin_PD1) },
    #endif

    #ifdef GPIO_PD2_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD2), MP_ROM_PTR(&pin_PD2) },
    #endif

    #ifdef GPIO_PD3_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD3), MP_ROM_PTR(&pin_PD3) },
    #endif

    #ifdef GPIO_PD4_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD4), MP_ROM_PTR(&pin_PD4) },
    #endif

    #ifdef GPIO_PD5_EXISTS
    { MP_ROM_QSTR(MP_QSTR_PD5), MP_ROM_PTR(&pin_PD5) }
    #endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
