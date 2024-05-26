// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/nvm/__init__.h"
#include "shared-bindings/nvm/ByteArray.h"

//| """Non-volatile memory
//|
//| The `nvm` module allows you to store whatever raw bytes you wish in a
//| reserved section non-volatile memory.
//|
//| Note that this module can't be imported and used directly. The sole
//| instance of :class:`ByteArray` is available at
//| :attr:`microcontroller.nvm`."""
static const mp_rom_map_elem_t nvm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nvm) },
    { MP_ROM_QSTR(MP_QSTR_ByteArray),   MP_ROM_PTR(&nvm_bytearray_type) },
};

static MP_DEFINE_CONST_DICT(nvm_module_globals, nvm_module_globals_table);

const mp_obj_module_t nvm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&nvm_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_nvm, nvm_module);
