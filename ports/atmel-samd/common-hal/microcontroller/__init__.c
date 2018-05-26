/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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
#include "hal/include/hal_atomic.h"
#include "py/runtime.h"

#include "reset.h"

#include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

// Interrupt flags that will be saved and restored during disable/Enable
// interrupt functions below.
volatile hal_atomic_t flags;
void common_hal_mcu_disable_interrupts(void) {
    atomic_enter_critical(&flags);
}

void common_hal_mcu_enable_interrupts(void) {
    atomic_leave_critical(&flags);
}

extern uint32_t _ezero;

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    // Set up the defaults.
    _bootloader_dbl_tap = DBL_TAP_MAGIC_QUICK_BOOT;
    _ezero = CIRCUITPY_CANARY_WORD;

    if (runmode == RUNMODE_BOOTLOADER) {
        if (!bootloader_available()) {
            mp_raise_ValueError("Cannot reset into bootloader because no bootloader is present.");
        }
        // Pretend to be the first of the two reset presses needed to enter the
        // bootloader. That way one reset will end in the bootloader.
        _bootloader_dbl_tap = DBL_TAP_MAGIC;
    } else if (runmode == RUNMODE_SAFE_MODE) {
        _ezero = CIRCUITPY_SOFTWARE_SAFE_MODE;
    }
}

void common_hal_mcu_reset(void) {
    reset();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

// NVM is only available on Express boards for now.
#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
    .start_address = (uint8_t*) (FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE)
};
#endif

// This maps MCU pin names to pin objects.
STATIC const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
// Pins in datasheet order.
#if defined(PIN_PA00) && !defined(IGNORE_PIN_PA00)
    { MP_ROM_QSTR(MP_QSTR_PA00), MP_ROM_PTR(&pin_PA00) },
#endif
#if defined(PIN_PA01) && !defined(IGNORE_PIN_PA01)
    { MP_ROM_QSTR(MP_QSTR_PA01), MP_ROM_PTR(&pin_PA01) },
#endif
#if defined(PIN_PA02) && !defined(IGNORE_PIN_PA02)
    { MP_ROM_QSTR(MP_QSTR_PA02), MP_ROM_PTR(&pin_PA02) },
#endif
#if defined(PIN_PA03) && !defined(IGNORE_PIN_PA03)
    { MP_ROM_QSTR(MP_QSTR_PA03), MP_ROM_PTR(&pin_PA03) },
#endif
#if defined(PIN_PB04) && !defined(IGNORE_PIN_PB04)
    { MP_ROM_QSTR(MP_QSTR_PB04), MP_ROM_PTR(&pin_PB04) },
#endif
#if defined(PIN_PB05) && !defined(IGNORE_PIN_PB05)
    { MP_ROM_QSTR(MP_QSTR_PB05), MP_ROM_PTR(&pin_PB05) },
#endif
#if defined(PIN_PB06) && !defined(IGNORE_PIN_PB06)
    { MP_ROM_QSTR(MP_QSTR_PB06), MP_ROM_PTR(&pin_PB06) },
#endif
#if defined(PIN_PB07) && !defined(IGNORE_PIN_PB07)
    { MP_ROM_QSTR(MP_QSTR_PB07), MP_ROM_PTR(&pin_PB07) },
#endif
#if defined(PIN_PB08) && !defined(IGNORE_PIN_PB08)
    { MP_ROM_QSTR(MP_QSTR_PB08), MP_ROM_PTR(&pin_PB08) },
#endif
#if defined(PIN_PB09) && !defined(IGNORE_PIN_PB09)
    { MP_ROM_QSTR(MP_QSTR_PB09), MP_ROM_PTR(&pin_PB09) },
#endif
#if defined(PIN_PA04) && !defined(IGNORE_PIN_PA04)
    { MP_ROM_QSTR(MP_QSTR_PA04), MP_ROM_PTR(&pin_PA04) },
#endif
#if defined(PIN_PA05) && !defined(IGNORE_PIN_PA05)
    { MP_ROM_QSTR(MP_QSTR_PA05), MP_ROM_PTR(&pin_PA05) },
#endif
#if defined(PIN_PA06) && !defined(IGNORE_PIN_PA06)
    { MP_ROM_QSTR(MP_QSTR_PA06), MP_ROM_PTR(&pin_PA06) },
#endif
#if defined(PIN_PA07) && !defined(IGNORE_PIN_PA07)
    { MP_ROM_QSTR(MP_QSTR_PA07), MP_ROM_PTR(&pin_PA07) },
#endif
#if defined(PIN_PA08) && !defined(IGNORE_PIN_PA08)
    { MP_ROM_QSTR(MP_QSTR_PA08), MP_ROM_PTR(&pin_PA08) },
#endif
#if defined(PIN_PA09) && !defined(IGNORE_PIN_PA09)
    { MP_ROM_QSTR(MP_QSTR_PA09), MP_ROM_PTR(&pin_PA09) },
#endif
#if defined(PIN_PA10) && !defined(IGNORE_PIN_PA10)
    { MP_ROM_QSTR(MP_QSTR_PA10), MP_ROM_PTR(&pin_PA10) },
#endif
#if defined(PIN_PA11) && !defined(IGNORE_PIN_PA11)
    { MP_ROM_QSTR(MP_QSTR_PA11), MP_ROM_PTR(&pin_PA11) },
#endif
#if defined(PIN_PB10) && !defined(IGNORE_PIN_PB10)
    { MP_ROM_QSTR(MP_QSTR_PB10), MP_ROM_PTR(&pin_PB10) },
#endif
#if defined(PIN_PB11) && !defined(IGNORE_PIN_PB11)
    { MP_ROM_QSTR(MP_QSTR_PB11), MP_ROM_PTR(&pin_PB11) },
#endif
#if defined(PIN_PB12) && !defined(IGNORE_PIN_PB12)
    { MP_ROM_QSTR(MP_QSTR_PB12), MP_ROM_PTR(&pin_PB12) },
#endif
#if defined(PIN_PB13) && !defined(IGNORE_PIN_PB13)
    { MP_ROM_QSTR(MP_QSTR_PB13), MP_ROM_PTR(&pin_PB13) },
#endif
#if defined(PIN_PB14) && !defined(IGNORE_PIN_PB14)
    { MP_ROM_QSTR(MP_QSTR_PB14), MP_ROM_PTR(&pin_PB14) },
#endif

// Second page.
#if defined(PIN_PB15) && !defined(IGNORE_PIN_PB15)
    { MP_ROM_QSTR(MP_QSTR_PB15), MP_ROM_PTR(&pin_PB15) },
#endif
#if defined(PIN_PA12) && !defined(IGNORE_PIN_PA12)
    { MP_ROM_QSTR(MP_QSTR_PA12), MP_ROM_PTR(&pin_PA12) },
#endif
#if defined(PIN_PA13) && !defined(IGNORE_PIN_PA13)
    { MP_ROM_QSTR(MP_QSTR_PA13), MP_ROM_PTR(&pin_PA13) },
#endif
#if defined(PIN_PA14) && !defined(IGNORE_PIN_PA14)
    { MP_ROM_QSTR(MP_QSTR_PA14), MP_ROM_PTR(&pin_PA14) },
#endif
#if defined(PIN_PA15) && !defined(IGNORE_PIN_PA15)
    { MP_ROM_QSTR(MP_QSTR_PA15), MP_ROM_PTR(&pin_PA15) },
#endif
#if defined(PIN_PA16) && !defined(IGNORE_PIN_PA16)
    { MP_ROM_QSTR(MP_QSTR_PA16), MP_ROM_PTR(&pin_PA16) },
#endif
#if defined(PIN_PA17) && !defined(IGNORE_PIN_PA17)
    { MP_ROM_QSTR(MP_QSTR_PA17), MP_ROM_PTR(&pin_PA17) },
#endif
#if defined(PIN_PA18) && !defined(IGNORE_PIN_PA18)
    { MP_ROM_QSTR(MP_QSTR_PA18), MP_ROM_PTR(&pin_PA18) },
#endif
#if defined(PIN_PA19) && !defined(IGNORE_PIN_PA19)
    { MP_ROM_QSTR(MP_QSTR_PA19), MP_ROM_PTR(&pin_PA19) },
#endif
#if defined(PIN_PB16) && !defined(IGNORE_PIN_PB16)
    { MP_ROM_QSTR(MP_QSTR_PB16), MP_ROM_PTR(&pin_PB16) },
#endif
#if defined(PIN_PB17) && !defined(IGNORE_PIN_PB17)
    { MP_ROM_QSTR(MP_QSTR_PB17), MP_ROM_PTR(&pin_PB17) },
#endif
#if defined(PIN_PA20) && !defined(IGNORE_PIN_PA20)
    { MP_ROM_QSTR(MP_QSTR_PA20), MP_ROM_PTR(&pin_PA20) },
#endif
#if defined(PIN_PA21) && !defined(IGNORE_PIN_PA21)
    { MP_ROM_QSTR(MP_QSTR_PA21), MP_ROM_PTR(&pin_PA21) },
#endif
#if defined(PIN_PA22) && !defined(IGNORE_PIN_PA22)
    { MP_ROM_QSTR(MP_QSTR_PA22), MP_ROM_PTR(&pin_PA22) },
#endif
#if defined(PIN_PA23) && !defined(IGNORE_PIN_PA23)
    { MP_ROM_QSTR(MP_QSTR_PA23), MP_ROM_PTR(&pin_PA23) },
#endif
#if defined(PIN_PA24) && !defined(IGNORE_PIN_PA24)
    { MP_ROM_QSTR(MP_QSTR_PA24), MP_ROM_PTR(&pin_PA24) },
#endif
#if defined(PIN_PA25) && !defined(IGNORE_PIN_PA25)
    { MP_ROM_QSTR(MP_QSTR_PA25), MP_ROM_PTR(&pin_PA25) },
#endif
#if defined(PIN_PB22) && !defined(IGNORE_PIN_PB22)
    { MP_ROM_QSTR(MP_QSTR_PB22), MP_ROM_PTR(&pin_PB22) },
#endif
#if defined(PIN_PB23) && !defined(IGNORE_PIN_PB23)
    { MP_ROM_QSTR(MP_QSTR_PB23), MP_ROM_PTR(&pin_PB23) },
#endif
#if defined(PIN_PA27) && !defined(IGNORE_PIN_PA27)
    { MP_ROM_QSTR(MP_QSTR_PA27), MP_ROM_PTR(&pin_PA27) },
#endif
#if defined(PIN_PA28) && !defined(IGNORE_PIN_PA28)
    { MP_ROM_QSTR(MP_QSTR_PA28), MP_ROM_PTR(&pin_PA28) },
#endif
#if defined(PIN_PA30) && !defined(IGNORE_PIN_PA30)
    { MP_ROM_QSTR(MP_QSTR_PA30), MP_ROM_PTR(&pin_PA30) },
#endif
#if defined(PIN_PA31) && !defined(IGNORE_PIN_PA31)
    { MP_ROM_QSTR(MP_QSTR_PA31), MP_ROM_PTR(&pin_PA31) },
#endif
#if defined(PIN_PB30) && !defined(IGNORE_PIN_PB30)
    { MP_ROM_QSTR(MP_QSTR_PB30), MP_ROM_PTR(&pin_PB30) },
#endif
#if defined(PIN_PB31) && !defined(IGNORE_PIN_PB31)
    { MP_ROM_QSTR(MP_QSTR_PB31), MP_ROM_PTR(&pin_PB31) },
#endif
#if defined(PIN_PB00) && !defined(IGNORE_PIN_PB00)
    { MP_ROM_QSTR(MP_QSTR_PB00), MP_ROM_PTR(&pin_PB00) },
#endif
#if defined(PIN_PB01) && !defined(IGNORE_PIN_PB01)
    { MP_ROM_QSTR(MP_QSTR_PB01), MP_ROM_PTR(&pin_PB01) },
#endif
#if defined(PIN_PB02) && !defined(IGNORE_PIN_PB02)
    { MP_ROM_QSTR(MP_QSTR_PB02), MP_ROM_PTR(&pin_PB02) },
#endif
#if defined(PIN_PB03) && !defined(IGNORE_PIN_PB03)
    { MP_ROM_QSTR(MP_QSTR_PB03), MP_ROM_PTR(&pin_PB03) }
#endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
