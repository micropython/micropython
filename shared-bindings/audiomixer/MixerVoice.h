// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 DeanM for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "shared-module/audiomixer/MixerVoice.h"
#include "shared-module/audiomixer/Mixer.h"

extern const mp_obj_type_t audiomixer_mixer_type;
extern const mp_obj_type_t audiomixer_mixervoice_type;

void common_hal_audiomixer_mixervoice_construct(audiomixer_mixervoice_obj_t *self);
void common_hal_audiomixer_mixervoice_set_parent(audiomixer_mixervoice_obj_t *self, audiomixer_mixer_obj_t *parent);
void common_hal_audiomixer_mixervoice_play(audiomixer_mixervoice_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiomixer_mixervoice_stop(audiomixer_mixervoice_obj_t *self);
mp_float_t common_hal_audiomixer_mixervoice_get_level(audiomixer_mixervoice_obj_t *self);
void common_hal_audiomixer_mixervoice_set_level(audiomixer_mixervoice_obj_t *self, mp_float_t gain);

bool common_hal_audiomixer_mixervoice_get_playing(audiomixer_mixervoice_obj_t *self);
