// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 DeanM for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiomixer/__init__.h"
#include "shared-module/audiomixer/Mixer.h"
#if CIRCUITPY_SYNTHIO
#include "shared-module/synthio/block.h"
#endif

typedef struct {
    mp_obj_base_t base;
    audiomixer_mixer_obj_t *parent;
    mp_obj_t sample;
    bool loop;
    bool more_data;
    // Start at the current level instead of waiting for a zero crossing.
    bool snap_to_level;
    uint32_t *remaining_buffer;
    uint32_t buffer_length;
    #if CIRCUITPY_SYNTHIO
    synthio_block_slot_t level;
    synthio_block_slot_t panning;
    #else
    uint16_t level;
    int16_t panning;
    #endif
    int32_t active_lo_level;
    int32_t active_hi_level;
} audiomixer_mixervoice_obj_t;
