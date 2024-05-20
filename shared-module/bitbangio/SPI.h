// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    digitalio_digitalinout_obj_t clock;
    digitalio_digitalinout_obj_t mosi;
    digitalio_digitalinout_obj_t miso;
    uint32_t delay_half;
    bool has_miso : 1;
    bool has_mosi : 1;
    uint8_t polarity : 1;
    uint8_t phase : 1;
    volatile bool locked : 1;
} bitbangio_spi_obj_t;
