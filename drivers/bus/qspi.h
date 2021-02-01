/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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
#ifndef MICROPY_INCLUDED_DRIVERS_BUS_QSPI_H
#define MICROPY_INCLUDED_DRIVERS_BUS_QSPI_H

#include "py/mphal.h"

#define MP_SPI_ADDR_IS_32B(addr) (addr & 0xff000000)

enum {
    MP_QSPI_IOCTL_INIT,
    MP_QSPI_IOCTL_DEINIT,
    MP_QSPI_IOCTL_BUS_ACQUIRE,
    MP_QSPI_IOCTL_BUS_RELEASE,
};

typedef struct _mp_qspi_proto_t {
    int (*ioctl)(void *self, uint32_t cmd);
    void (*write_cmd_data)(void *self, uint8_t cmd, size_t len, uint32_t data);
    void (*write_cmd_addr_data)(void *self, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src);
    uint32_t (*read_cmd)(void *self, uint8_t cmd, size_t len);
    void (*read_cmd_qaddr_qdata)(void *self, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest);
} mp_qspi_proto_t;

typedef struct _mp_soft_qspi_obj_t {
    mp_hal_pin_obj_t cs;
    mp_hal_pin_obj_t clk;
    mp_hal_pin_obj_t io0;
    mp_hal_pin_obj_t io1;
    mp_hal_pin_obj_t io2;
    mp_hal_pin_obj_t io3;
} mp_soft_qspi_obj_t;

extern const mp_qspi_proto_t mp_soft_qspi_proto;

static inline uint8_t mp_spi_set_addr_buff(uint8_t *buf, uint32_t addr) {
    if (MP_SPI_ADDR_IS_32B(addr)) {
        buf[0] = addr >> 24;
        buf[1] = addr >> 16;
        buf[2] = addr >> 8;
        buf[3] = addr;
        return 4;
    } else {
        buf[0] = addr >> 16;
        buf[1] = addr >> 8;
        buf[2] = addr;
        return 3;
    }
}

#endif // MICROPY_INCLUDED_DRIVERS_BUS_QSPI_H
