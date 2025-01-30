/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_DRV_SPI_H
#define MP_PORT_MCX_DRV_SPI_H

#include "py/obj.h"

typedef struct drv_spi_config_type {
    uint32_t frequency;
    uint8_t cpol;
    uint8_t cpha;
    uint8_t frame_size;
    bool lsbfirst;
} drv_spi_config_t;

typedef struct drv_spi_transfer_type {
    const uint8_t *tx_data;
    uint8_t *rx_data;
    uint32_t len;
} drv_spi_transfer_t;

typedef struct drv_spi_type {
    uint8_t id;
    void *instance;
} drv_spi_t;

int drv_spi_init(drv_spi_t *spi, uint8_t id, drv_spi_config_t *cfg);
int drv_spi_transfer(drv_spi_t *spi, drv_spi_transfer_t *transfer);

#endif
