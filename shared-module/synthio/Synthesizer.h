// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/synthio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    synthio_synth_t synth;
    mp_obj_t blocks;
} synthio_synthesizer_obj_t;


// These are not available from Python because it may be called in an interrupt.
void synthio_synthesizer_reset_buffer(synthio_synthesizer_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t synthio_synthesizer_get_buffer(synthio_synthesizer_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length); // length in bytes

void synthio_synthesizer_get_buffer_structure(synthio_synthesizer_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);
