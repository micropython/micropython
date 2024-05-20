// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *spi;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t extra_clocks;
    digitalio_digitalinout_obj_t *chip_select;
    bool cs_active_value;
} adafruit_bus_device_spidevice_obj_t;
