// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "shared/external_flash/device.h"

#include "shared-bindings/busio/SPI.h"

extern busio_spi_obj_t supervisor_flash_spi_bus; // Used to share SPI bus on some boards

// This API is implemented for both normal SPI peripherals and QSPI peripherals.

bool spi_flash_command(uint8_t command);
bool spi_flash_read_command(uint8_t command, uint8_t *response, uint32_t length);
bool spi_flash_write_command(uint8_t command, uint8_t *data, uint32_t length);
bool spi_flash_sector_command(uint8_t command, uint32_t address);
bool spi_flash_write_data(uint32_t address, uint8_t *data, uint32_t data_length);
bool spi_flash_read_data(uint32_t address, uint8_t *data, uint32_t data_length);
void spi_flash_init(void);
void spi_flash_init_device(const external_flash_device *device);
