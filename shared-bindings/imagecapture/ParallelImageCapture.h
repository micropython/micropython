/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#pragma once

#include "common-hal/microcontroller/Pin.h"

typedef struct imagecapture_parallelimagecapture_obj imagecapture_parallelimagecapture_obj_t;
extern const mp_obj_type_t imagecapture_parallelimagecapture_type;

// if only the first element of data_pins is non-NULL, the pins are sequential in microcontroller pin numbering.
void common_hal_imagecapture_parallelimagecapture_construct(imagecapture_parallelimagecapture_obj_t *self,
    const uint8_t data_pins[],
    uint8_t data_count,
    const mcu_pin_obj_t *data_clock,
    const mcu_pin_obj_t *vertical_sync,
    const mcu_pin_obj_t *horizontal_sync);
void common_hal_imagecapture_parallelimagecapture_deinit(imagecapture_parallelimagecapture_obj_t *self);
bool common_hal_imagecapture_parallelimagecapture_deinited(imagecapture_parallelimagecapture_obj_t *self);
void common_hal_imagecapture_parallelimagecapture_capture(imagecapture_parallelimagecapture_obj_t *self, void *buffer, size_t bufsize);
