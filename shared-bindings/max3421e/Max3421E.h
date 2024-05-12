/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
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

#pragma once

#include "shared-module/max3421e/Max3421E.h"

extern const mp_obj_type_t max3421e_max3421e_type;

void common_hal_max3421e_max3421e_construct(max3421e_max3421e_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *irq,
    uint32_t baudrate);

bool common_hal_max3421e_max3421e_deinited(max3421e_max3421e_obj_t *self);
void common_hal_max3421e_max3421e_deinit(max3421e_max3421e_obj_t *self);

// TinyUSB requires these three functions.

// API to control MAX3421 SPI CS
extern void tuh_max3421_spi_cs_api(uint8_t rhport, bool active);

// API to transfer data with MAX3421 SPI
// Either tx_buf or rx_buf can be NULL, which means transfer is write or read only
extern bool tuh_max3421_spi_xfer_api(uint8_t rhport, uint8_t const *tx_buf, uint8_t *rx_buf, size_t xfer_bytes);

// API to enable/disable MAX3421 INTR pin interrupt
extern void tuh_max3421_int_api(uint8_t rhport, bool enabled);
