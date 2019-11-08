/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include "storage.h"
#include "qspi.h"

// Shared cache for first and second SPI block devices
STATIC mp_spiflash_cache_t spi_bdev_cache;

// First external SPI flash uses software QSPI interface

STATIC const mp_soft_qspi_obj_t soft_qspi_bus = {
    .cs = MICROPY_HW_SPIFLASH_CS,
    .clk = MICROPY_HW_SPIFLASH_SCK,
    .io0 = MICROPY_HW_SPIFLASH_IO0,
    .io1 = MICROPY_HW_SPIFLASH_IO1,
    .io2 = MICROPY_HW_SPIFLASH_IO2,
    .io3 = MICROPY_HW_SPIFLASH_IO3,
};

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = (void*)&soft_qspi_bus,
    .bus.u_qspi.proto = &mp_soft_qspi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

// Second external SPI flash uses hardware QSPI interface

const mp_spiflash_config_t spiflash2_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = NULL,
    .bus.u_qspi.proto = &qspi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev2;
