// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/synthio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    synthio_synth_t synth;
    mp_buffer_info_t track;
    // invariant: after initial startup, pos always points just after an encoded duration, i.e., at a midi message (or at EOF)
    size_t pos;
    mp_int_t error_location;
    uint32_t tempo;
} synthio_miditrack_obj_t;


// These are not available from Python because it may be called in an interrupt.
void synthio_miditrack_reset_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t synthio_miditrack_get_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length); // length in bytes

void synthio_miditrack_get_buffer_structure(synthio_miditrack_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);
