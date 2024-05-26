// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/usb/__init__.h"
#include "shared-bindings/usb/core/__init__.h"
#include "supervisor/usb.h"

//| """PyUSB-compatible USB host API
//|
//| The `usb` is a subset of PyUSB that allows you to communicate to USB devices.
//| """
//|

static mp_rom_map_elem_t usb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_usb) },
    { MP_ROM_QSTR(MP_QSTR_core),          MP_OBJ_FROM_PTR(&usb_core_module) },
};

static MP_DEFINE_CONST_DICT(usb_module_globals, usb_module_globals_table);

const mp_obj_module_t usb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb, usb_module);
