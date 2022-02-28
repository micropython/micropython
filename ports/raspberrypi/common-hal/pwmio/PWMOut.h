/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_RASPBERRY_PI_COMMON_HAL_PWMIO_PWMOUT_H
#define MICROPY_INCLUDED_RASPBERRY_PI_COMMON_HAL_PWMIO_PWMOUT_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t slice;        // 0-7
    uint8_t ab_channel;   // 0-1: A or B slice channel
    bool variable_frequency;
    uint16_t duty_cycle;
    uint32_t actual_frequency;
    uint16_t top;
} pwmio_pwmout_obj_t;

void pwmout_reset(void);
// Private API for AudioPWMOut.
void pwmio_pwmout_set_top(pwmio_pwmout_obj_t *self, uint16_t top);
// Private APIs for RGBMatrix
enum pwmout_result_t pwmout_allocate(uint8_t slice, uint8_t ab_channel, bool variable_frequency, uint32_t frequency);
void pwmout_free(uint8_t slice, uint8_t ab_channel);
void pwmout_never_reset(uint8_t slice, uint8_t ab_channel);
void pwmout_reset_ok(uint8_t slice, uint8_t ab_channel);

// Private API for countio to claim both ab_channels on a slice
bool pwmio_claim_slice_ab_channels(uint8_t slice);
void pwmio_release_slice_ab_channels(uint8_t slice);

#endif // MICROPY_INCLUDED_RASPBERRY_PI_COMMON_HAL_PWMIO_PWMOUT_H
