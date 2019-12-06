/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/tick.h"
#include "shared-module/gamepad/__init__.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "nrf.h"

void SysTick_Handler(void) {
    // Do things common to all ports when the tick occurs
    supervisor_tick();
}

void tick_init() {
    uint32_t ticks_per_ms = common_hal_mcu_processor_get_frequency() / 1000;
    SysTick_Config(ticks_per_ms); // interrupt is enabled
}

void tick_delay(uint32_t us) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;
    uint32_t us_between_ticks = SysTick->VAL / ticks_per_us;
    uint64_t start_ms = supervisor_ticks_ms64();
    while (us > 1000) {
        while (supervisor_ticks_ms64() == start_ms) {}
        us -= us_between_ticks;
        start_ms = supervisor_ticks_ms64();
        us_between_ticks = 1000;
    }
    while (SysTick->VAL > ((us_between_ticks - us) * ticks_per_us)) {}
}

// us counts down!
void current_tick(uint64_t* ms, uint32_t* us_until_ms) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;
    *ms = supervisor_ticks_ms64();
    *us_until_ms = SysTick->VAL / ticks_per_us;
}

void wait_until(uint64_t ms, uint32_t us_until_ms) {
    uint32_t ticks_per_us = common_hal_mcu_processor_get_frequency() / 1000 / 1000;
    while(supervisor_ticks_ms64() <= ms && SysTick->VAL / ticks_per_us >= us_until_ms) {}
}
