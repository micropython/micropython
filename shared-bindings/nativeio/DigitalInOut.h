/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#ifndef __MICROPY_INCLUDED_SHARED_BINDINGS_NATIVEIO_DIGITALINOUT_H__
#define __MICROPY_INCLUDED_SHARED_BINDINGS_NATIVEIO_DIGITALINOUT_H__

#include "common-hal/microcontroller/types.h"
#include "common-hal/nativeio/types.h"

extern const mp_obj_type_t nativeio_digitalinout_type;

enum digitalinout_direction_t {
    DIRECTION_IN,
    DIRECTION_OUT
};

enum digitalinout_pull_t {
    PULL_NONE,
    PULL_UP,
    PULL_DOWN
};

enum digitalinout_drive_mode_t {
    DRIVE_MODE_PUSH_PULL,
    DRIVE_MODE_OPEN_DRAIN
};

typedef enum {
    DIGITALINOUT_OK,
    DIGITALINOUT_PIN_BUSY
} digitalinout_result_t;

digitalinout_result_t common_hal_nativeio_digitalinout_construct(nativeio_digitalinout_obj_t* self, const mcu_pin_obj_t* pin);
void common_hal_nativeio_digitalinout_deinit(nativeio_digitalinout_obj_t* self);
void common_hal_nativeio_digitalinout_switch_to_input(nativeio_digitalinout_obj_t* self, enum digitalinout_pull_t pull);
void common_hal_nativeio_digitalinout_switch_to_output(nativeio_digitalinout_obj_t* self, bool value, enum digitalinout_drive_mode_t drive_mode);
enum digitalinout_direction_t common_hal_nativeio_digitalinout_get_direction(nativeio_digitalinout_obj_t* self);
void common_hal_nativeio_digitalinout_set_value(nativeio_digitalinout_obj_t* self, bool value);
bool common_hal_nativeio_digitalinout_get_value(nativeio_digitalinout_obj_t* self);
void common_hal_nativeio_digitalinout_set_drive_mode(nativeio_digitalinout_obj_t* self, enum digitalinout_drive_mode_t drive_mode);
enum digitalinout_drive_mode_t common_hal_nativeio_digitalinout_get_drive_mode(nativeio_digitalinout_obj_t* self);
void common_hal_nativeio_digitalinout_set_pull(nativeio_digitalinout_obj_t* self, enum digitalinout_pull_t pull);
enum digitalinout_pull_t common_hal_nativeio_digitalinout_get_pull(nativeio_digitalinout_obj_t* self);

#endif // __MICROPY_INCLUDED_SHARED_BINDINGS_NATIVEIO_DIGITALINOUT_H__
