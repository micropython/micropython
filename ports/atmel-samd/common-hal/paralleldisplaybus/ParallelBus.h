// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *bus;
    digitalio_digitalinout_obj_t command;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t reset;
    digitalio_digitalinout_obj_t write;
    digitalio_digitalinout_obj_t read;
    uint8_t data0_pin;
    PortGroup *write_group;
    uint32_t write_mask;
} paralleldisplaybus_parallelbus_obj_t;
