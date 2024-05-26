// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "extmod/vfs_fat.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *data_pin;
    uint32_t sample_rate;
    uint8_t serializer;
    uint8_t clock_unit;
    uint8_t bytes_per_sample;
    uint8_t bit_depth;
    uint8_t gclk;
} audiobusio_pdmin_obj_t;

void pdmin_reset(void);

void pdmin_evsys_handler(void);

void pdmin_background(void);
