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

#ifndef MICROPY_HW_SPI_ADDR_IS_32BIT
#define MICROPY_HW_SPI_ADDR_IS_32BIT(addr) (addr & 0xff000000)
#endif

enum {
    MP_QSPI_IOCTL_INIT,
    MP_QSPI_IOCTL_DEINIT,
    MP_QSPI_IOCTL_BUS_ACQUIRE,
    MP_QSPI_IOCTL_BUS_RELEASE,
    MP_QSPI_IOCTL_MEMORY_MODIFIED,
};

typedef struct _mp_qspi_proto_t {
    int (*ioctl)(void *self, uint32_t cmd, uintptr_t arg);
    int (*write_cmd_data)(void *self, uint8_t cmd, size_t len, uint32_t data);
    int (*write_cmd_addr_data)(void *self, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src);
    int (*read_cmd)(void *self, uint8_t cmd, size_t len, uint32_t *dest);
    int (*read_cmd_qaddr_qdata)(void *self, uint8_t cmd, uint32_t addr, uint8_t num_dummy, size_t len, uint8_t *dest);
    int (*direct_read)(void *self, uint32_t addr, size_t len, uint8_t *dest); // can be NULL if direct read not supported
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
    if (MICROPY_HW_SPI_ADDR_IS_32BIT(addr)) {
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
