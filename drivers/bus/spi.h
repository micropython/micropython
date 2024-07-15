/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Damien P. George
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
#ifndef MICROPY_INCLUDED_DRIVERS_BUS_SPI_H
#define MICROPY_INCLUDED_DRIVERS_BUS_SPI_H

#include "py/mphal.h"

enum {
    MP_SPI_IOCTL_INIT,
    MP_SPI_IOCTL_DEINIT,
};

typedef struct _mp_spi_proto_t {
    int (*ioctl)(void *self, uint32_t cmd);
    void (*transfer)(void *self, size_t len, const uint8_t *src, uint8_t *dest);
} mp_spi_proto_t;

typedef struct _mp_soft_spi_obj_t {
    uint32_t delay_half; // microsecond delay for half SCK period
    uint8_t polarity;
    uint8_t phase;
    uint8_t firstbit;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
} mp_soft_spi_obj_t;

extern const mp_spi_proto_t mp_soft_spi_proto;

int mp_soft_spi_ioctl(void *self, uint32_t cmd);
void mp_soft_spi_transfer(void *self, size_t len, const uint8_t *src, uint8_t *dest);

#endif // MICROPY_INCLUDED_DRIVERS_BUS_SPI_H
