/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H

#include "driver/spi_master.h"
#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;

    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    const mcu_pin_obj_t *clock;

    spi_host_device_t host_id;

    uint8_t bits;
    uint8_t phase;
    uint8_t polarity;
    uint32_t baudrate;

    bool has_lock;
} busio_spi_obj_t;

void spi_reset(void);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H
