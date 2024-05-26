// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/digitalio/DriveMode.h"

//| class DriveMode:
//|     """Defines the drive mode of a digital pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the drive mode used when outputting
//|         digital values."""
//|         ...
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

static const mp_rom_map_elem_t digitalio_drive_mode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PUSH_PULL),    MP_ROM_PTR(&digitalio_drive_mode_push_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),   MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj) },
};
static MP_DEFINE_CONST_DICT(digitalio_drive_mode_locals_dict, digitalio_drive_mode_locals_dict_table);

static void digitalio_drive_mode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr drive_mode = MP_QSTR_PUSH_PULL;
    if (self_in == MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj)) {
        drive_mode = MP_QSTR_OPEN_DRAIN;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_DriveMode, drive_mode);
}

MP_DEFINE_CONST_OBJ_TYPE(
    digitalio_drive_mode_type,
    MP_QSTR_DriveMode,
    MP_TYPE_FLAG_NONE,
    print, digitalio_drive_mode_print,
    locals_dict, &digitalio_drive_mode_locals_dict
    );
