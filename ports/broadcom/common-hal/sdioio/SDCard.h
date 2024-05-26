// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "lib/sdmmc/include/sdmmc_types.h"

typedef struct {
    mp_obj_base_t base;
    sdmmc_host_t host_info;
    sdmmc_card_t card_info;
    uint32_t frequency;
    uint32_t capacity;
    uint8_t num_data;
    bool state_programming;
    bool has_lock;
    bool init;
    uint8_t command_pin;
    uint8_t clock_pin;
    uint8_t data_pins[4];
} sdioio_sdcard_obj_t;
