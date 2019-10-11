/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <sys/boardctl.h>

#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

void common_hal_mcu_disable_interrupts(void) {
    __asm volatile ("cpsid i" : : : "memory");
}

void common_hal_mcu_enable_interrupts(void) {
    __asm volatile ("cpsie i" : : : "memory");
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if(runmode == RUNMODE_BOOTLOADER) {
        mp_raise_ValueError(translate("Cannot reset into bootloader because no bootloader is present."));
    } else if(runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush();
    boardctl(BOARDIOC_RESET, 0);
}

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_D0) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_D1) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_D2) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_D3) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_D4) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_D5) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_D6) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_D7) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_D8) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_D9) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_D10) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_D11) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_D12) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_D13) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_D14) },
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_D15) },
    { MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_D16) },
    { MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_D17) },
    { MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_D18) },
    { MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_D19) },
    { MP_ROM_QSTR(MP_QSTR_D20), MP_ROM_PTR(&pin_D20) },
    { MP_ROM_QSTR(MP_QSTR_D21), MP_ROM_PTR(&pin_D21) },
    { MP_ROM_QSTR(MP_QSTR_D22), MP_ROM_PTR(&pin_D22) },
    { MP_ROM_QSTR(MP_QSTR_D23), MP_ROM_PTR(&pin_D23) },
    { MP_ROM_QSTR(MP_QSTR_D24), MP_ROM_PTR(&pin_D24) },
    { MP_ROM_QSTR(MP_QSTR_D25), MP_ROM_PTR(&pin_D25) },
    { MP_ROM_QSTR(MP_QSTR_D26), MP_ROM_PTR(&pin_D26) },
    { MP_ROM_QSTR(MP_QSTR_D27), MP_ROM_PTR(&pin_D27) },
    { MP_ROM_QSTR(MP_QSTR_D28), MP_ROM_PTR(&pin_D28) },
    { MP_ROM_QSTR(MP_QSTR_LED0), MP_ROM_PTR(&pin_LED0) },
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_LED1) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_LED2) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_LED3) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_A0) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_A1) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_A2) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_A3) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_A4) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_A5) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
