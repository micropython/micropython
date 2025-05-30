/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2022 Damien P. George
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

#include "py/obj.h"
#include "storage.h"
#include "spi.h"
#include "xspi.h"

#if BUILDING_MBOOT

// Mboot doesn't support hardware SPI, so use software SPI instead.

static const mp_soft_spi_obj_t soft_spi_bus = {
    .delay_half = MICROPY_HW_SOFTSPI_MIN_DELAY,
    .polarity = 0,
    .phase = 0,
    .sck = MBOOT_SPIFLASH_SCK,
    .mosi = MBOOT_SPIFLASH_MOSI,
    .miso = MBOOT_SPIFLASH_MISO,
};

const mp_spiflash_config_t board_mboot_spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_SPI,
    .bus.u_spi.cs = MBOOT_SPIFLASH_CS,
    .bus.u_spi.data = (void *)&soft_spi_bus,
    .bus.u_spi.proto = &mp_soft_spi_proto,
};

mp_spiflash_t board_mboot_spiflash;

#elif defined(MICROPY_HW_BDEV_SPIFLASH)

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
#error "Cannot enable MICROPY_HW_SPIFLASH_ENABLE_CACHE"
//static mp_spiflash_cache_t spi_bdev_cache;
#endif

// External SPI flash uses hardware XSPI interface (in 1-line mode).
const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI, // spiflash driver doesn't yet support XSPI directly
    .bus.u_qspi.data = (void *)&xspi_flash2,
    .bus.u_qspi.proto = &xspi_proto,
    //#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
    //.cache = &spi_bdev_cache,
    //#endif
    //.cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

#endif
