// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "shared/runtime/context_manager_helpers.h"

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

//| class Direction:
//|     """Defines the direction of a digital pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define which direction the digital values are
//|         going."""
//|         ...
//|     INPUT: Direction
//|     """Read digital data in"""
//|
//|     OUTPUT: Direction
//|     """Write digital data out"""
//|
const mp_obj_type_t digitalio_direction_type;

const digitalio_direction_obj_t digitalio_direction_input_obj = {
    { &digitalio_direction_type },
};

const digitalio_direction_obj_t digitalio_direction_output_obj = {
    { &digitalio_direction_type },
};

static const mp_rom_map_elem_t digitalio_direction_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_INPUT),    MP_ROM_PTR(&digitalio_direction_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_OUTPUT),   MP_ROM_PTR(&digitalio_direction_output_obj) },
};
static MP_DEFINE_CONST_DICT(digitalio_direction_locals_dict, digitalio_direction_locals_dict_table);

static void digitalio_direction_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr direction = MP_QSTR_INPUT;
    if (self_in == MP_ROM_PTR(&digitalio_direction_output_obj)) {
        direction = MP_QSTR_OUTPUT;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_Direction, direction);
}

MP_DEFINE_CONST_OBJ_TYPE(
    digitalio_direction_type,
    MP_QSTR_Direction,
    MP_TYPE_FLAG_NONE,
    print, digitalio_direction_print,
    locals_dict, &digitalio_direction_locals_dict
    );
