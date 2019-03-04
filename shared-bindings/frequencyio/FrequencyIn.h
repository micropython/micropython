/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_FREQUENCYIO_FREQUENCYIN_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_FREQUENCYIO_FREQUENCYIN_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/frequencyio/FrequencyIn.h"

extern const mp_obj_type_t frequencyio_frequencyin_type;

extern void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t capture_period);
extern void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self);
extern bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t *self);
extern uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self);
extern uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_FREQUENCYIO_FREQUENCYIN_H
