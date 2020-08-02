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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_AUDIOBUSIO_I2SOUT_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_AUDIOBUSIO_I2SOUT_H

#include "common-hal/microcontroller/Pin.h"

#include "audio_dma.h"
#include "py/obj.h"

// Some boards don't implement I2SOut, so suppress any routines from here.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

// We don't bit pack because we'll only have two at most. Its better to save code size instead.
typedef struct {
    mp_obj_base_t base;
    bool left_justified;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    uint8_t clock_unit;
    uint8_t serializer;
    uint8_t gclk;
    bool playing;
    audio_dma_t dma;
} audiobusio_i2sout_obj_t;

void i2sout_reset(void);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_AUDIOBUSIO_I2SOUT_H
