/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_H

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_spi_m_sync.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct spi_m_sync_descriptor spi_desc;
    bool has_lock;
    uint8_t clock_pin;
    uint8_t MOSI_pin;
    uint8_t MISO_pin;
} busio_spi_obj_t;

void reset_sercoms(void);
void never_reset_sercom(Sercom *sercom);


#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_H
