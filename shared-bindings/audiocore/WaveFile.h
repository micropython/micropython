// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
