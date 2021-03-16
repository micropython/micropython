/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dave Putz for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PULSEIO_PULSEIN_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PULSEIO_PULSEIN_H

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_pio/include/hardware/pio.h"
#include "common-hal/rp2pio/StateMachine.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin;
    uint16_t *buffer;
    uint16_t maxlen;
    bool idle_state;
    volatile uint16_t start;
    volatile uint16_t len;
    rp2pio_statemachine_obj_t state_machine;
} pulseio_pulsein_obj_t;

void pulsein_reset(void);
void common_hal_pulseio_pulsein_interrupt();

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PULSEIO_PULSEIN_H
