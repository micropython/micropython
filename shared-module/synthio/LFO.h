// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/synthio/block.h"

typedef struct synthio_lfo_obj {
    synthio_block_base_t base;
    bool once, interpolate;

    synthio_block_slot_t rate, scale, offset, phase_offset;
    mp_float_t accum;

    mp_obj_t waveform_obj;
    mp_buffer_info_t waveform_bufinfo;
} synthio_lfo_obj_t;

// Update the value inside the lfo slot if the value is an LFO, returning the new value
mp_float_t synthio_block_slot_get(synthio_block_slot_t *block_slot);
// the same, but the output is constrained to be between lo and hi
mp_float_t synthio_block_slot_get_limited(synthio_block_slot_t *block_slot, mp_float_t lo, mp_float_t hi);
// the same, but the output is constrained to be between lo and hi and converted to an integer with 15 fractional bits
int32_t synthio_block_slot_get_scaled(synthio_block_slot_t *block_slot, mp_float_t lo, mp_float_t hi);

// Assign an object (which may be a float or a synthio_block_obj_t) to an block slot
void synthio_block_assign_slot(mp_obj_t obj, synthio_block_slot_t *block_slot, qstr arg_name);
