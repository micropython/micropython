// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "audio_dma.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *left_channel;
    audio_dma_t left_dma;
    #ifdef SAM_D5X_E5X
    const mcu_pin_obj_t *right_channel;
    audio_dma_t right_dma;
    #endif
    uint8_t tc_index;

    uint8_t tc_to_dac_event_channel;
    bool playing;
    uint16_t quiescent_value;
} audioio_audioout_obj_t;

void audioout_background(void);
