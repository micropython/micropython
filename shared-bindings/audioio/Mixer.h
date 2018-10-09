/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_MIXER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_MIXER_H

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/audioio/Mixer.h"
#include "shared-bindings/audioio/RawSample.h"

extern const mp_obj_type_t audioio_mixer_type;

void common_hal_audioio_mixer_construct(audioio_mixer_obj_t* self,
                                        uint8_t voice_count,
                                        uint32_t buffer_size,
                                        uint8_t bits_per_sample,
                                        bool samples_signed,
                                        uint8_t channel_count,
                                        uint32_t sample_rate);

void common_hal_audioio_mixer_deinit(audioio_mixer_obj_t* self);
bool common_hal_audioio_mixer_deinited(audioio_mixer_obj_t* self);
void common_hal_audioio_mixer_play(audioio_mixer_obj_t* self, mp_obj_t sample, uint8_t voice, bool loop);
void common_hal_audioio_mixer_stop_voice(audioio_mixer_obj_t* self, uint8_t voice);

bool common_hal_audioio_mixer_get_playing(audioio_mixer_obj_t* self);
uint32_t common_hal_audioio_mixer_get_sample_rate(audioio_mixer_obj_t* self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_MIXER_H
