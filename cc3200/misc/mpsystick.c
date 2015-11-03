/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "mpsystick.h"
#include "systick.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif


bool sys_tick_has_passed(uint32_t start_tick, uint32_t delay_ms) {
    return mp_hal_ticks_ms() - start_tick >= delay_ms;
}

// waits until at least delay_ms milliseconds have passed from the sampling of
// startTick. Handles overflow properly. Assumes stc was taken from
// mp_hal_ticks_ms() some time before calling this function.
void sys_tick_wait_at_least(uint32_t start_tick, uint32_t delay_ms) {
#ifdef USE_FREERTOS
    vTaskDelay (delay_ms / portTICK_PERIOD_MS);
#else
    while (!sys_tick_has_passed(start_tick, delay_ms)) {
        __WFI(); // enter sleep mode, waiting for interrupt
    }
#endif
}

// The SysTick timer counts down at HAL_FCPU_HZ, so we can use that knowledge
// to grab a microsecond counter.
// We assume that mp_hal_ticks_ms returns milliseconds.
uint32_t sys_tick_get_microseconds(void) {
    mp_uint_t irq_state = disable_irq();
    uint32_t counter = SysTickValueGet();
    uint32_t milliseconds = mp_hal_ticks_ms();
    enable_irq(irq_state);

    uint32_t load = SysTickPeriodGet();
    counter = load - counter; // Convert from decrementing to incrementing
    return (milliseconds * 1000) + ((counter * 1000) / load);
}
