// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "hal_mci_sync.h"

typedef struct {
    mp_obj_base_t base;
    struct mci_sync_desc IO_BUS;
    uint32_t frequency;
    uint32_t capacity;
    uint8_t num_data : 3, state_programming : 1, has_lock : 1;
    uint8_t command_pin;
    uint8_t clock_pin;
    uint8_t data_pins[4];
} sdioio_sdcard_obj_t;
