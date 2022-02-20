/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "systick.h"

volatile uint32_t systick_ms = 0;

systick_dispatch_t systick_dispatch_table[SYSTICK_DISPATCH_NUM_SLOTS];

void SysTick_Handler(void) {
    // Instead of calling HAL_IncTick we do the increment here of the counter.
    // This is purely for efficiency, since SysTick is called 1000 times per
    // second at the highest interrupt priority.
    uint32_t uw_tick = systick_ms + 1;
    systick_ms = uw_tick;

    // Dispatch to any registered handlers in a cycle
    systick_dispatch_t f = systick_dispatch_table[uw_tick & (SYSTICK_DISPATCH_NUM_SLOTS - 1)];
    if (f != NULL) {
        f(uw_tick);
    }
}

bool systick_has_passed(uint32_t start_tick, uint32_t delay_ms) {
    return systick_ms - start_tick >= delay_ms;
}

// waits until at least delay_ms milliseconds have passed from the sampling of
// startTick. Handles overflow properly. Assumes stc was taken from
// HAL_GetTick() some time before calling this function.
void systick_wait_at_least(uint32_t start_tick, uint32_t delay_ms) {
    while (!systick_has_passed(start_tick, delay_ms)) {
        __WFI(); // enter sleep mode, waiting for interrupt
    }
}
