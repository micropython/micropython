// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/synthio/Synthesizer.h"

extern const mp_obj_type_t synthio_synthesizer_type;

void common_hal_synthio_synthesizer_construct(synthio_synthesizer_obj_t *self,
    uint32_t sample_rate, int channel_count, mp_obj_t waveform_obj,
    mp_obj_t envelope_obj);
void common_hal_synthio_synthesizer_deinit(synthio_synthesizer_obj_t *self);
bool common_hal_synthio_synthesizer_deinited(synthio_synthesizer_obj_t *self);
uint32_t common_hal_synthio_synthesizer_get_sample_rate(synthio_synthesizer_obj_t *self);
uint8_t common_hal_synthio_synthesizer_get_bits_per_sample(synthio_synthesizer_obj_t *self);
uint8_t common_hal_synthio_synthesizer_get_channel_count(synthio_synthesizer_obj_t *self);
void common_hal_synthio_synthesizer_release(synthio_synthesizer_obj_t *self, mp_obj_t to_release);
void common_hal_synthio_synthesizer_press(synthio_synthesizer_obj_t *self, mp_obj_t to_press);
void common_hal_synthio_synthesizer_retrigger(synthio_synthesizer_obj_t *self, mp_obj_t to_retrigger);
void common_hal_synthio_synthesizer_release_all(synthio_synthesizer_obj_t *self);
mp_obj_t common_hal_synthio_synthesizer_get_pressed_notes(synthio_synthesizer_obj_t *self);
mp_obj_t common_hal_synthio_synthesizer_get_blocks(synthio_synthesizer_obj_t *self);
envelope_state_e common_hal_synthio_synthesizer_note_info(synthio_synthesizer_obj_t *self, mp_obj_t note, mp_float_t *vol_out);
