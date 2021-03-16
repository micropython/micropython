/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_AUDIOOUT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_AUDIOOUT_H

#include "common-hal/audioio/AudioOut.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/audiocore/RawSample.h"

extern const mp_obj_type_t audioio_audioout_type;

// left_channel will always be non-NULL but right_channel may be for mono output.
void common_hal_audioio_audioout_construct(audioio_audioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t default_value);

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_play(audioio_audioout_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audioio_audioout_stop(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_pause(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_resume(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_get_paused(audioio_audioout_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_AUDIOOUT_H
