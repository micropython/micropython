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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_WAVEFILE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_WAVEFILE_H

#include "py/obj.h"
#include "extmod/vfs_fat.h"

#include "shared-module/audiocore/WaveFile.h"

extern const mp_obj_type_t audioio_wavefile_type;

void common_hal_audioio_wavefile_construct(audioio_wavefile_obj_t *self,
    pyb_file_obj_t *file, uint8_t *buffer, size_t buffer_size);

void common_hal_audioio_wavefile_deinit(audioio_wavefile_obj_t *self);
bool common_hal_audioio_wavefile_deinited(audioio_wavefile_obj_t *self);
uint32_t common_hal_audioio_wavefile_get_sample_rate(audioio_wavefile_obj_t *self);
void common_hal_audioio_wavefile_set_sample_rate(audioio_wavefile_obj_t *self, uint32_t sample_rate);
uint8_t common_hal_audioio_wavefile_get_bits_per_sample(audioio_wavefile_obj_t *self);
uint8_t common_hal_audioio_wavefile_get_channel_count(audioio_wavefile_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_AUDIOIO_WAVEFILE_H
