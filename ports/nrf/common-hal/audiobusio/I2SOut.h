/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_AUDIOBUSIO_I2SOUT_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_AUDIOBUSIO_I2SOUT_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    mp_obj_t *sample;
    uint8_t *buffers[2];
    uint8_t *sample_data, *sample_end;

    uint16_t buffer_length;
    uint16_t sample_rate;
    uint32_t hold_value;

    uint8_t next_buffer;
    uint8_t bit_clock_pin_number;
    uint8_t word_select_pin_number;
    uint8_t data_pin_number;

    uint8_t channel_count;
    uint8_t bytes_per_sample;

    bool left_justified : 1;
    bool playing : 1;
    bool stopping : 1;
    bool paused : 1;
    bool loop : 1;
    bool samples_signed : 1;
    bool single_buffer : 1;
} audiobusio_i2sout_obj_t;

void i2s_reset(void);
void i2s_background(void);

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_AUDIOBUSIO_I2SOUT_H
