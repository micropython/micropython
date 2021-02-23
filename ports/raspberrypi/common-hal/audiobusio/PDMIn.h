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

#ifndef MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H
#define MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H

#include "common-hal/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"

#include "extmod/vfs_fat.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t sample_rate;
    uint8_t serializer;
    uint8_t clock_unit;
    uint8_t bytes_per_sample;
    uint8_t bit_depth;
    rp2pio_statemachine_obj_t state_machine;
} audiobusio_pdmin_obj_t;

void pdmin_reset(void);

void pdmin_background(void);

#endif // MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H
