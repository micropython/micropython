// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include "py/obj.h"
#include "peripherals/pins.h"

typedef struct MemsAudio_t MemsAudio;
typedef struct MemsAudio_STM32L4SAIPDM_t MemsAudio_STM32L4SAIPDM;

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *data_pin;
    uint32_t sample_rate;
    uint8_t bit_depth;
    bool mono;
    uint8_t oversample;
    MemsAudio *audio;
    MemsAudio_STM32L4SAIPDM *audio_impl;
    /**
     * @brief Flag to indicate from the ISR that recording is complete.
     */
    volatile bool recording_complete;
} audiobusio_pdmin_obj_t;
