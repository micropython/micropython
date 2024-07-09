// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/uheap/__init__.h"

//| """Heap size analysis"""
//|

//| def info(object: object) -> int:
//|     """Prints memory debugging info for the given object and returns the
//|     estimated size."""
//|     ...
//|
static mp_obj_t uheap_info(mp_obj_t obj) {
    uint32_t size = shared_module_uheap_info(obj);

    return MP_OBJ_NEW_SMALL_INT(size);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uheap_info_obj, uheap_info);

static const mp_rom_map_elem_t uheap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uheap) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&uheap_info_obj) },
};

static MP_DEFINE_CONST_DICT(uheap_module_globals, uheap_module_globals_table);

const mp_obj_module_t uheap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&uheap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_uheap, uheap_module);
