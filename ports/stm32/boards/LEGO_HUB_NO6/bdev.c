/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#define CMD_EXIT_4_BYTE_ADDRESS_MODE (0xE9)

STATIC const spi_proto_cfg_t spi_bus = {
    .spi = &spi_obj[1], // SPI2 hardware peripheral
    .baudrate = 25000000,
    .polarity = 0,
    .phase = 0,
    .bits = 8,
    .firstbit = SPI_FIRSTBIT_MSB,
};

STATIC mp_spiflash_cache_t spi_bdev_cache;

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_SPI,
    .bus.u_spi.cs = MICROPY_HW_SPIFLASH_CS,
    .bus.u_spi.data = (void *)&spi_bus,
    .bus.u_spi.proto = &spi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

int32_t board_bdev_ioctl(void) {
    int32_t ret = spi_bdev_ioctl(&spi_bdev, BDEV_IOCTL_INIT, (uint32_t)&spiflash_config);

    // Exit 4-byte address mode
    uint8_t cmd = CMD_EXIT_4_BYTE_ADDRESS_MODE;
    mp_hal_pin_write(MICROPY_HW_SPIFLASH_CS, 0);
    spi_proto.transfer(MP_OBJ_FROM_PTR(&spi_bus), 1, &cmd, NULL);
    mp_hal_pin_write(MICROPY_HW_SPIFLASH_CS, 1);

    return ret;
}
