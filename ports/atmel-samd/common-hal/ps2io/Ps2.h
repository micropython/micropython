/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PS2IO_PS2_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PS2IO_PS2_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t channel;
    uint8_t clk_pin;
    uint8_t data_pin;

    uint8_t state;
    uint64_t last_raw_ticks;

    uint16_t bits;
    bool parity;
    uint8_t bitcount;

    uint8_t buffer[16];
    uint8_t bufcount;
    uint8_t bufposr;
    uint8_t bufposw;

    uint16_t last_errors;

    bool waiting_cmd_response;
    uint8_t cmd_response;
} ps2io_ps2_obj_t;

void ps2_interrupt_handler(uint8_t channel);

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_PS2IO_PS2_H
