/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOBUSIO_I2SOUT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOBUSIO_I2SOUT_H

#include "common-hal/audiobusio/I2SOut.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t audiobusio_i2sout_type;

// Some boards don't have the I2SOut pins available.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select, const mcu_pin_obj_t *data,
    bool left_justified);

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOBUSIO_I2SOUT_H
