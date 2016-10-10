/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_SPI_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_SPI_H

#include "py/obj.h"
#include "py/mphal.h"

// SPI protocol
typedef struct _mp_machine_spi_p_t {
    void (*transfer)(mp_obj_base_t *obj, size_t len, const uint8_t *src, uint8_t *dest);
} mp_machine_spi_p_t;

typedef struct _mp_machine_soft_spi_obj_t {
    mp_obj_base_t base;
    uint32_t delay_half; // microsecond delay for half SCK period
    uint8_t polarity;
    uint8_t phase;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
} mp_machine_soft_spi_obj_t;

void mp_machine_soft_spi_transfer(mp_obj_base_t *self, size_t len, const uint8_t *src, uint8_t *dest);

MP_DECLARE_CONST_FUN_OBJ(mp_machine_spi_read_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_machine_spi_readinto_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_machine_spi_write_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_machine_spi_write_readinto_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_SPI_H
