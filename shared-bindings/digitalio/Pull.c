// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-bindings/digitalio/Pull.h"

//| class Pull:
//|     """Defines the pull of a digital input pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the pull value, if any, used while reading
//|         digital values in."""
//|         ...
//|     UP: Pull
//|     """When the input line isn't being driven the pull up can pull the state
//|     of the line high so it reads as true."""
//|
//|     DOWN: Pull
//|     """When the input line isn't being driven the pull down can pull the
//|     state of the line low so it reads as false."""
//|
const mp_obj_type_t digitalio_pull_type;

const digitalio_pull_obj_t digitalio_pull_up_obj = {
    { &digitalio_pull_type },
};

const digitalio_pull_obj_t digitalio_pull_down_obj = {
    { &digitalio_pull_type },
};

static const mp_rom_map_elem_t digitalio_pull_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UP),    MP_ROM_PTR(&digitalio_pull_up_obj) },
    { MP_ROM_QSTR(MP_QSTR_DOWN),  MP_ROM_PTR(&digitalio_pull_down_obj) },
};
static MP_DEFINE_CONST_DICT(digitalio_pull_locals_dict, digitalio_pull_locals_dict_table);

static void digitalio_pull_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr pull = MP_QSTR_UP;
    if (self_in == MP_ROM_PTR(&digitalio_pull_down_obj)) {
        pull = MP_QSTR_DOWN;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_Pull, pull);
}

MP_DEFINE_CONST_OBJ_TYPE(
    digitalio_pull_type,
    MP_QSTR_Pull,
    MP_TYPE_FLAG_NONE,
    print, digitalio_pull_print,
    locals_dict, &digitalio_pull_locals_dict
    );

digitalio_pull_t validate_pull(mp_rom_obj_t obj, qstr arg_name) {
    if (obj == MP_ROM_PTR(&digitalio_pull_up_obj)) {
        return PULL_UP;
    } else if (obj == MP_ROM_PTR(&digitalio_pull_down_obj)) {
        return PULL_DOWN;
    }
    if (obj == MP_ROM_NONE) {
        return PULL_NONE;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), arg_name, MP_QSTR_Pull, MP_QSTR_None, mp_obj_get_type(obj)->name);
}
