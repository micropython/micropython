/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DIGITALINOUT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DIGITALINOUT_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

extern const mp_obj_type_t digitalio_digitalinout_type;

typedef enum {
    DIGITALINOUT_OK,
    DIGITALINOUT_PIN_BUSY,
    DIGITALINOUT_INPUT_ONLY
} digitalinout_result_t;

digitalinout_result_t common_hal_digitalio_digitalinout_construct(digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin);
void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self);
bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_switch_to_input(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull);
digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(digitalio_digitalinout_obj_t *self, bool value, digitalio_drive_mode_t drive_mode);
digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_set_value(digitalio_digitalinout_obj_t *self, bool value);
bool common_hal_digitalio_digitalinout_get_value(digitalio_digitalinout_obj_t *self);
digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(digitalio_digitalinout_obj_t *self, digitalio_drive_mode_t drive_mode);
digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_set_pull(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull);
digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_never_reset(digitalio_digitalinout_obj_t *self);
digitalio_digitalinout_obj_t *assert_digitalinout(mp_obj_t obj);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DIGITALINOUT_H
