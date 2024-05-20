// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"
#include "driver/gpio.h"

typedef struct {
    mp_obj_base_t base;
    gpio_num_t clk_pin;
    gpio_num_t data_pin;

    uint8_t state;
    uint64_t last_raw_ticks;

    uint16_t bits;
    bool parity;
    uint8_t bitcount;

    uint8_t buffer[16];
    uint8_t bufcount;
    uint8_t bufposr;
    uint8_t bufposw;

    uint16_t last_errors;

    bool waiting_cmd_response;
    uint8_t cmd_response;
} ps2io_ps2_obj_t;

void ps2_reset(void);
