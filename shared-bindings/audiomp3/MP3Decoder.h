// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "extmod/vfs_fat.h"

#include "shared-module/audiomp3/MP3Decoder.h"

extern const mp_obj_type_t audiomp3_mp3file_type;

void common_hal_audiomp3_mp3file_construct(audiomp3_mp3file_obj_t *self,
    mp_obj_t stream, uint8_t *buffer, size_t buffer_size);

void common_hal_audiomp3_mp3file_set_file(audiomp3_mp3file_obj_t *self, mp_obj_t stream);
void common_hal_audiomp3_mp3file_deinit(audiomp3_mp3file_obj_t *self);
bool common_hal_audiomp3_mp3file_deinited(audiomp3_mp3file_obj_t *self);
uint32_t common_hal_audiomp3_mp3file_get_sample_rate(audiomp3_mp3file_obj_t *self);
void common_hal_audiomp3_mp3file_set_sample_rate(audiomp3_mp3file_obj_t *self, uint32_t sample_rate);
uint8_t common_hal_audiomp3_mp3file_get_bits_per_sample(audiomp3_mp3file_obj_t *self);
uint8_t common_hal_audiomp3_mp3file_get_channel_count(audiomp3_mp3file_obj_t *self);
float common_hal_audiomp3_mp3file_get_rms_level(audiomp3_mp3file_obj_t *self);
uint32_t common_hal_audiomp3_mp3file_get_samples_decoded(audiomp3_mp3file_obj_t *self);
