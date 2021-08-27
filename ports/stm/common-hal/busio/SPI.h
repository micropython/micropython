/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SPI_H

#include "common-hal/microcontroller/Pin.h"

#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    SPI_HandleTypeDef handle;
    bool has_lock;
    const mcu_periph_obj_t *sck;
    const mcu_periph_obj_t *mosi;
    const mcu_periph_obj_t *miso;
    const mcu_periph_obj_t *nss;
    uint32_t baudrate;
    uint16_t prescaler;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
} busio_spi_obj_t;

void spi_reset(void);

#endif // MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SPI_H
