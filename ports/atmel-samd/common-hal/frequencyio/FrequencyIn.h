/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_FREQUENCYIO_FREQUENCYIN_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_FREQUENCYIO_FREQUENCYIN_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t tc_index;
    uint8_t pin;
    uint8_t channel;
    uint8_t event_channel;
    uint32_t frequency;
    volatile uint64_t last_ns;
    float factor;
    uint32_t capture_period;
    uint8_t TC_IRQ;
    volatile bool errored_too_fast;
} frequencyio_frequencyin_obj_t;

void frequencyin_interrupt_handler(uint8_t index);
void frequencyin_emergency_cancel_capture(uint8_t index);
void frequencyin_reference_tc_init(void);
void frequencyin_reference_tc_enable(bool enable);
bool frequencyin_reference_tc_enabled(void);
#ifdef SAM_D5X_E5X
void frequencyin_samd51_start_dpll(void);
void frequencyin_samd51_stop_dpll(void);
#endif


#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_FREQUENCYIO_FREQUENCYIN_H
