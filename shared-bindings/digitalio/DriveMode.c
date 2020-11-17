/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/digitalio/DriveMode.h"

//| class DriveMode:
//|     """Defines the drive mode of a digital pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the drive mode used when outputting
//|         digital values."""
//|         ...
//|
//|     PUSH_PULL: DriveMode
//|     """Output both high and low digital values"""
//|
//|     OPEN_DRAIN: DriveMode
//|     """Output low digital values but go into high z for digital high. This is
//|     useful for i2c and other protocols that share a digital line."""
//|
const mp_obj_type_t digitalio_drive_mode_type;

const digitalio_drive_mode_obj_t digitalio_drive_mode_push_pull_obj = {
    { &digitalio_drive_mode_type },
};

const digitalio_drive_mode_obj_t digitalio_drive_mode_open_drain_obj = {
    { &digitalio_drive_mode_type },
};

STATIC const mp_rom_map_elem_t digitalio_drive_mode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PUSH_PULL),    MP_ROM_PTR(&digitalio_drive_mode_push_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),   MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_drive_mode_locals_dict, digitalio_drive_mode_locals_dict_table);

STATIC void digitalio_drive_mode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr drive_mode = MP_QSTR_PUSH_PULL;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj)) {
        drive_mode = MP_QSTR_OPEN_DRAIN;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_DriveMode, drive_mode);
}

const mp_obj_type_t digitalio_drive_mode_type = {
    { &mp_type_type },
    .name = MP_QSTR_DriveMode,
    .print = digitalio_drive_mode_print,
    .locals_dict = (mp_obj_t)&digitalio_drive_mode_locals_dict,
};
