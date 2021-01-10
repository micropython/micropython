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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H

#include "common-hal/microcontroller/Pin.h"

#include "components/driver/include/driver/spi_common_internal.h"
#include "components/soc/include/hal/spi_hal.h"
#include "components/soc/include/hal/spi_types.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t* clock_pin;
    const mcu_pin_obj_t* MOSI_pin;
    const mcu_pin_obj_t* MISO_pin;
    spi_host_device_t host_id;
    spi_bus_lock_dev_handle_t lock;
    spi_hal_context_t hal_context;
    spi_hal_timing_conf_t timing_conf;
    intr_handle_t interrupt;
    uint32_t target_frequency;
    int32_t real_frequency;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    bool has_lock;
    bool connected_through_gpio;
} busio_spi_obj_t;

void spi_reset(void);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_BUSIO_SPI_H
