// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
        // This is very very bad because it means there was mismatched disable/enables.
        reset_into_safe_mode(SAFE_MODE_INTERRUPT_ERROR);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    irq_setie(1);
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
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

static const mp_rom_map_elem_t mcu_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TOUCH1), MP_ROM_PTR(&pin_TOUCH1) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH2), MP_ROM_PTR(&pin_TOUCH2) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH3), MP_ROM_PTR(&pin_TOUCH3) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH4), MP_ROM_PTR(&pin_TOUCH4) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
