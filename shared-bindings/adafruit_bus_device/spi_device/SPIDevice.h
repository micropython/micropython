// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Komus
//
// SPDX-License-Identifier: MIT

// Machine is the HAL for low-level, hardware accelerated functions. It is not
// meant to simplify APIs, its only meant to unify them so that other modules
// do not require port specific logic.
//
// This file includes externs for all functions a port should implement to
// support the machine module.

#pragma once

#include "py/obj.h"

#include "shared-module/adafruit_bus_device/spi_device/SPIDevice.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t adafruit_bus_device_spidevice_type;

// Initializes the hardware peripheral.
extern void common_hal_adafruit_bus_device_spidevice_construct(adafruit_bus_device_spidevice_obj_t *self, busio_spi_obj_t *spi,  digitalio_digitalinout_obj_t *cs,
    bool cs_active_value, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t extra_clocks);
extern mp_obj_t common_hal_adafruit_bus_device_spidevice_enter(adafruit_bus_device_spidevice_obj_t *self);
extern void common_hal_adafruit_bus_device_spidevice_exit(adafruit_bus_device_spidevice_obj_t *self);
