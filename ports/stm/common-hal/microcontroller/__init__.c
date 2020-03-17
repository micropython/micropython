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

//tested divisor value for busy loop in us delay
#define LOOP_TICKS 12

STATIC uint32_t get_us(void) {
    uint32_t ticks_per_us = HAL_RCC_GetSysClockFreq()/1000000;
    uint32_t micros, sys_cycles;
    do {
        micros = supervisor_ticks_ms32();
        sys_cycles = SysTick->VAL; //counts backwards
    } while (micros != supervisor_ticks_ms32()); //try again if ticks_ms rolled over
    return (micros * 1000) + (ticks_per_us * 1000 - sys_cycles) / ticks_per_us;
}

void common_hal_mcu_delay_us(uint32_t delay) {
    if (__get_PRIMASK() == 0x00000000) {
        //by default use ticks_ms
        uint32_t start = get_us();
        while (get_us()-start < delay) {
            __asm__ __volatile__("nop");
        }
    } else {
        //when SysTick is disabled, approximate with busy loop
        const uint32_t ucount = HAL_RCC_GetSysClockFreq() / 1000000 * delay / LOOP_TICKS;
        for (uint32_t count = 0; ++count <= ucount;) {
        }
    }
}

volatile uint32_t nesting_count = 0;

void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // "HardFault".
        asm("bkpt");
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    __enable_irq();
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
  if(runmode == RUNMODE_SAFE_MODE)
    safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
}

void common_hal_mcu_reset(void) {
    filesystem_flush(); //TODO: implement as part of flash improvements
    NVIC_SystemReset();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};
