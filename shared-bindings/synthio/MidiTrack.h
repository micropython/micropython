// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/synthio/MidiTrack.h"
#include "py/obj.h"

extern const mp_obj_type_t synthio_miditrack_type;

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self, const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate, mp_obj_t waveform_obj, mp_obj_t filter_obj, mp_obj_t envelope_obj);

void common_hal_synthio_miditrack_deinit(synthio_miditrack_obj_t *self);
bool common_hal_synthio_miditrack_deinited(synthio_miditrack_obj_t *self);
uint32_t common_hal_synthio_miditrack_get_sample_rate(synthio_miditrack_obj_t *self);
uint8_t common_hal_synthio_miditrack_get_bits_per_sample(synthio_miditrack_obj_t *self);
uint8_t common_hal_synthio_miditrack_get_channel_count(synthio_miditrack_obj_t *self);
mp_int_t common_hal_synthio_miditrack_get_error_location(synthio_miditrack_obj_t *self);
