/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

#include "csr.h"
#include "irq.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    // if (__get_PRIMASK() == 0x00000000) {
    //     //by default use ticks_ms
    //     uint32_t start = get_us();
    //     while (get_us()-start < delay) {
    //         __asm__ __volatile__("nop");
    //     }
    // } else {
    //     //when SysTick is disabled, approximate with busy loop
    //     const uint32_t ucount = HAL_RCC_GetSysClockFreq() / 1000000 * delay / LOOP_TICKS;
    //     for (uint32_t count = 0; ++count <= ucount;) {
    //     }
    // }
}

volatile uint32_t nesting_count = 0;

__attribute__((section(".ramtext")))
void common_hal_mcu_disable_interrupts(void) {
    if (nesting_count == 0) {
        irq_setie(0);
    }
    nesting_count++;
}

__attribute__((section(".ramtext")))
void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // "HardFault".
        asm ("ebreak");
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    irq_setie(1);
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush(); // TODO: implement as part of flash improvements
    // NVIC_SystemReset();
    while (1) {
        ;
    }
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

const mcu_pin_obj_t pin_TOUCH1 = PIN(0);
const mcu_pin_obj_t pin_TOUCH2 = PIN(1);
const mcu_pin_obj_t pin_TOUCH3 = PIN(2);
const mcu_pin_obj_t pin_TOUCH4 = PIN(3);

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TOUCH1), MP_ROM_PTR(&pin_TOUCH1) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH2), MP_ROM_PTR(&pin_TOUCH2) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH3), MP_ROM_PTR(&pin_TOUCH3) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH4), MP_ROM_PTR(&pin_TOUCH4) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
