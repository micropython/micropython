// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    SD_HandleTypeDef handle;
    uint8_t num_data : 3, state_programming : 1;
    bool has_lock;
    const mcu_periph_obj_t *command;
    const mcu_periph_obj_t *clock;
    const mcu_periph_obj_t *data[4];
    uint32_t frequency;
    uint32_t capacity;
} sdioio_sdcard_obj_t;

void sdioio_reset(void);
