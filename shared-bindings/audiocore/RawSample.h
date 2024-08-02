// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiocore/RawSample.h"

extern const mp_obj_type_t audioio_rawsample_type;

void common_hal_audioio_rawsample_construct(audioio_rawsample_obj_t *self,
    uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate, bool single_buffer);

void common_hal_audioio_rawsample_deinit(audioio_rawsample_obj_t *self);
bool common_hal_audioio_rawsample_deinited(audioio_rawsample_obj_t *self);
uint32_t common_hal_audioio_rawsample_get_sample_rate(audioio_rawsample_obj_t *self);
uint8_t common_hal_audioio_rawsample_get_bits_per_sample(audioio_rawsample_obj_t *self);
uint8_t common_hal_audioio_rawsample_get_channel_count(audioio_rawsample_obj_t *self);
void common_hal_audioio_rawsample_set_sample_rate(audioio_rawsample_obj_t *self, uint32_t sample_rate);
