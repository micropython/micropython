/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 DeanM for Adafruit Industries
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
#ifndef SHARED_BINDINGS_AUDIOMIXER_MIXERVOICE_H_
#define SHARED_BINDINGS_AUDIOMIXER_MIXERVOICE_H_

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/audiocore/RawSample.h"

#include "shared-module/audiomixer/MixerVoice.h"
#include "shared-module/audiomixer/Mixer.h"

extern const mp_obj_type_t audiomixer_mixer_type;
extern const mp_obj_type_t audiomixer_mixervoice_type;

void common_hal_audiomixer_mixervoice_construct(audiomixer_mixervoice_obj_t *self);
void common_hal_audiomixer_mixervoice_set_parent(audiomixer_mixervoice_obj_t *self, audiomixer_mixer_obj_t *parent);
void common_hal_audiomixer_mixervoice_play(audiomixer_mixervoice_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiomixer_mixervoice_stop(audiomixer_mixervoice_obj_t *self);
float common_hal_audiomixer_mixervoice_get_level(audiomixer_mixervoice_obj_t *self);
void common_hal_audiomixer_mixervoice_set_level(audiomixer_mixervoice_obj_t *self, float gain);

bool common_hal_audiomixer_mixervoice_get_playing(audiomixer_mixervoice_obj_t *self);

#endif /* SHARED_BINDINGS_AUDIOMIXER_MIXERVOICE_H_ */
