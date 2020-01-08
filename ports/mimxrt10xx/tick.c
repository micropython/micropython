/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "tick.h"

#include "fsl_common.h"

#include "supervisor/shared/tick.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Processor.h"

void SysTick_Handler(void) {
    // SysTick interrupt handler called when the SysTick timer reaches zero
    // (every millisecond).
    common_hal_mcu_disable_interrupts();

    // Read the control register to reset the COUNTFLAG.
    (void) SysTick->CTRL;

    common_hal_mcu_enable_interrupts();

    // Do things common to all ports when the tick occurs
    supervisor_tick();
}

void tick_init() {
    uint32_t ticks_per_ms = common_hal_mcu_processor_get_frequency() / 1000;
    SysTick_Config(ticks_per_ms-1);
}

void tick_delay(uint32_t us) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;
    uint32_t us_until_next_tick = SysTick->VAL / ticks_per_us;
    uint32_t start_tick;
    while (us >= us_until_next_tick) {
        start_tick = SysTick->VAL;  // wait for SysTick->VAL to  RESET
        while (SysTick->VAL < start_tick) {}
        us -= us_until_next_tick;
        us_until_next_tick = 1000;
    }
    while (SysTick->VAL > ((us_until_next_tick - us) * ticks_per_us)) {}
}

// us counts down!
void current_tick(uint64_t* ms, uint32_t* us_until_ms) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;

    // We disable interrupts to prevent ticks_ms from changing while we grab it.
    common_hal_mcu_disable_interrupts();
    uint32_t tick_status = SysTick->CTRL;
    uint32_t current_us = SysTick->VAL;
    uint32_t tick_status2 = SysTick->CTRL;
    uint64_t current_ms = supervisor_ticks_ms64();
    // The second clause ensures our value actually rolled over. Its possible it hit zero between
    // the VAL read and CTRL read.
    if ((tick_status & SysTick_CTRL_COUNTFLAG_Msk) != 0 ||
        ((tick_status2 & SysTick_CTRL_COUNTFLAG_Msk) != 0 && current_us > ticks_per_us)) {
        current_ms++;
    }
    common_hal_mcu_enable_interrupts();
    *ms = current_ms;
    *us_until_ms = current_us / ticks_per_us;
}

void wait_until(uint64_t ms, uint32_t us_until_ms) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;
    while (supervisor_ticks_ms64() <= ms && SysTick->VAL / ticks_per_us >= us_until_ms) {}
}
