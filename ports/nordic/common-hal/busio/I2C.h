// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx_twim.h"

#include "py/obj.h"

typedef struct {
    nrfx_twim_t twim;
    bool in_use;
    volatile bool transferring;
    nrfx_twim_evt_type_t last_event_type;
    uint32_t timeout;
} twim_peripheral_t;

typedef struct {
    mp_obj_base_t base;
    twim_peripheral_t *twim_peripheral;
    bool has_lock;
    uint8_t scl_pin_number;
    uint8_t sda_pin_number;
} busio_i2c_obj_t;

void i2c_reset(void);
