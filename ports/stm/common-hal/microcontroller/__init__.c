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

#include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/port.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    uint32_t ticks_per_us = HAL_RCC_GetSysClockFreq() / 1000000UL;
    delay *= ticks_per_us;
    SysTick->VAL = 0UL;
    SysTick->LOAD = delay;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
    }
    SysTick->CTRL = 0UL;
}

volatile uint32_t nesting_count = 0;

void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables.
        reset_into_safe_mode(SAFE_MODE_INTERRUPT_ERROR);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    __enable_irq();
}

static bool next_reset_to_bootloader = false;

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
    }
    if (runmode == RUNMODE_BOOTLOADER) {
        next_reset_to_bootloader = true;
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush(); // TODO: implement as part of flash improvements

    if (next_reset_to_bootloader) {
        reset_to_bootloader();
    } else {
        NVIC_SystemReset();
    }
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
    .len = NVM_BYTEARRAY_BUFFER_SIZE,
    .start_address = (uint8_t *)(CIRCUITPY_INTERNAL_NVM_START_ADDR)
};
#endif
