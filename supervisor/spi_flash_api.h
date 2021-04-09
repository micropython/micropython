/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_SUPERVISOR_SPI_FLASH_H
#define MICROPY_INCLUDED_SUPERVISOR_SPI_FLASH_H

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

#endif  // MICROPY_INCLUDED_SUPERVISOR_SPI_FLASH_H
