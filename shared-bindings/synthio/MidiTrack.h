/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SYNTHIO_MIDITRACK_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SYNTHIO_MIDITRACK_H

#include "shared-module/synthio/MidiTrack.h"

extern const mp_obj_type_t synthio_miditrack_type;

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self,
    const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate);

void common_hal_synthio_miditrack_deinit(synthio_miditrack_obj_t *self);
bool common_hal_synthio_miditrack_deinited(synthio_miditrack_obj_t *self);
uint32_t common_hal_synthio_miditrack_get_sample_rate(synthio_miditrack_obj_t *self);
uint8_t common_hal_synthio_miditrack_get_bits_per_sample(synthio_miditrack_obj_t *self);
uint8_t common_hal_synthio_miditrack_get_channel_count(synthio_miditrack_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SYNTHIO_MIDITRACK_H
