// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jacob Rigby
//
// SPDX-License-Identifier: MIT

#pragma once

#include "driver/sdmmc_types.h"

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    sdmmc_card_t card;
    uint8_t slot;
    uint8_t num_data : 3, state_programming : 1, has_lock : 1;
    uint8_t command;
    uint8_t clock;
    uint8_t data[4];
    uint32_t frequency;
    uint32_t capacity;
} sdioio_sdcard_obj_t;

uint8_t get_slot_index(sdioio_sdcard_obj_t *);
