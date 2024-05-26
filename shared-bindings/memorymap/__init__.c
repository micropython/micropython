// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/memorymap/__init__.h"
#include "shared-bindings/memorymap/AddressRange.h"

//| """Raw memory map access
//|
//| The `memorymap` module allows you to read and write memory addresses in the
//| address space seen from the processor running CircuitPython. It is usually
//| the physical address space.
//| """
static const mp_rom_map_elem_t memorymap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_memorymap) },
    { MP_ROM_QSTR(MP_QSTR_AddressRange),   MP_ROM_PTR(&memorymap_addressrange_type) },
};

static MP_DEFINE_CONST_DICT(memorymap_module_globals, memorymap_module_globals_table);

const mp_obj_module_t memorymap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&memorymap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_memorymap, memorymap_module);
