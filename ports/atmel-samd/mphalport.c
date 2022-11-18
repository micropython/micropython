/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include <string.h>

#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/smallint.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"

#include "hal/include/hal_atomic.h"
#include "hal/include/hal_delay.h"
#include "hal/include/hal_gpio.h"
#include "hal/include/hal_sleep.h"
#include "sam.h"

#include "mpconfigboard.h"
#include "mphalport.h"
#include "reset.h"
#include "supervisor/shared/tick.h"

extern uint32_t common_hal_mcu_processor_get_frequency(void);

// Use mp_hal_delay_us() for timing of less than 1ms.
// Do a simple timing loop to wait for a certain number of microseconds.
// Can be used when interrupts are disabled, which makes tick_delay() unreliable.
//
// Testing done at 48 MHz on SAMD21 and 120 MHz on SAMD51, multiplication and division cancel out.
// But get the frequency just in case.
#ifdef SAMD21
#define DELAY_LOOP_ITERATIONS_PER_US ((10U * 48000000U) / common_hal_mcu_processor_get_frequency())
#endif
#ifdef SAM_D5X_E5X
#define DELAY_LOOP_ITERATIONS_PER_US ((30U * 120000000U) / common_hal_mcu_processor_get_frequency())
#endif

void mp_hal_delay_us(mp_uint_t delay) {
    for (uint32_t i = delay * DELAY_LOOP_ITERATIONS_PER_US; i > 0; i--) {
        asm volatile ("nop");
    }
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}
