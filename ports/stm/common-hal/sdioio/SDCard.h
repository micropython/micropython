/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SDIO_H
#define MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SDIO_H

#include "common-hal/microcontroller/Pin.h"

#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    SD_HandleTypeDef handle;
    uint8_t num_data : 3, state_programming : 1;
    bool has_lock;
    const mcu_periph_obj_t *command;
    const mcu_periph_obj_t *clock;
    const mcu_periph_obj_t *data[4];
    uint32_t frequency;
    uint32_t capacity;
} sdioio_sdcard_obj_t;

void sdioio_reset(void);

#endif // MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_SDIO_H
