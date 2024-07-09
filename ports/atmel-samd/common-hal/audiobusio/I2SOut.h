// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "audio_dma.h"
#include "py/obj.h"

// Some boards don't implement I2SOut, so suppress any routines from here.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

// We don't bit pack because we'll only have two at most. Its better to save code size instead.
typedef struct {
    mp_obj_base_t base;
    bool left_justified;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    uint8_t clock_unit;
    uint8_t serializer;
    uint8_t gclk;
    bool playing;
    audio_dma_t dma;
} audiobusio_i2sout_obj_t;

void i2sout_reset(void);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
