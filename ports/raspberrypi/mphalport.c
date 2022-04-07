/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "mpconfigboard.h"
#include "mphalport.h"
#include "supervisor/shared/tick.h"

#include "src/rp2_common/hardware_timer/include/hardware/timer.h"

extern uint32_t common_hal_mcu_processor_get_frequency(void);

void mp_hal_delay_us(mp_uint_t delay) {
    busy_wait_us_32(delay);
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}
