// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 DeanM for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiomixer/__init__.h"
#include "shared-module/audiomixer/Mixer.h"

typedef struct {
    mp_obj_base_t base;
    audiomixer_mixer_obj_t *parent;
    mp_obj_t sample;
    bool loop;
    bool more_data;
    uint32_t *remaining_buffer;
    uint32_t buffer_length;
    uint16_t level;
} audiomixer_mixervoice_obj_t;
