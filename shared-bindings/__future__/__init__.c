// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "extmod/vfs.h"
#include "py/mpstate.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/__future__/__init__.h"

//| """Language features module
//|
//| The `__future__` module is used by other Python implementations to
//| enable forward compatibility for features enabled by default in an upcoming version.
//| """
//|
//| annotations: Any
//| """In CPython, ``from __future__ import annotations``
//| indicates that evaluation of annotations is postponed, as described in PEP 563.
//| CircuitPython (and MicroPython) ignore annotations entirely, whether or not this feature is imported.
//| This is a limitation of CircuitPython and MicroPython for efficiency reasons.
//| """

static const mp_rom_map_elem_t future_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR___future__) },

    { MP_ROM_QSTR(MP_QSTR_annotations), mp_const_true },
};

static MP_DEFINE_CONST_DICT(future_module_globals, future_module_globals_table);

const mp_obj_module_t future_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&future_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR___future__, future_module);
