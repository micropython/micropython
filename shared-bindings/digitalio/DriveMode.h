// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DRIVEMODE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DRIVEMODE_H

#include "py/obj.h"

typedef enum {
    DRIVE_MODE_PUSH_PULL,
    DRIVE_MODE_OPEN_DRAIN
} digitalio_drive_mode_t;

typedef struct {
    mp_obj_base_t base;
} digitalio_drive_mode_obj_t;

extern const mp_obj_type_t digitalio_drive_mode_type;

extern const digitalio_drive_mode_obj_t digitalio_drive_mode_push_pull_obj;
extern const digitalio_drive_mode_obj_t digitalio_drive_mode_open_drain_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DIGITALIO_DRIVEMODE_H
