// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiomixer/Mixer.h"

extern const mp_obj_type_t audiomixer_mixer_type;
extern const mp_obj_type_t audiomixer_mixervoice_type;

void common_hal_audiomixer_mixer_construct(audiomixer_mixer_obj_t *self,
    uint8_t voice_count,
    uint32_t buffer_size,
    uint8_t bits_per_sample,
    bool samples_signed,
    uint8_t channel_count,
    uint32_t sample_rate);

void common_hal_audiomixer_mixer_deinit(audiomixer_mixer_obj_t *self);
bool common_hal_audiomixer_mixer_deinited(audiomixer_mixer_obj_t *self);

bool common_hal_audiomixer_mixer_get_playing(audiomixer_mixer_obj_t *self);
uint32_t common_hal_audiomixer_mixer_get_sample_rate(audiomixer_mixer_obj_t *self);
uint8_t common_hal_audiomixer_mixer_get_channel_count(audiomixer_mixer_obj_t *self);
uint8_t common_hal_audiomixer_mixer_get_bits_per_sample(audiomixer_mixer_obj_t *self);
