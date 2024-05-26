// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/memorymonitor/__init__.h"
#include "shared-bindings/memorymonitor/AllocationAlarm.h"
#include "shared-bindings/memorymonitor/AllocationSize.h"

//| """Memory monitoring helpers"""
//|

//| class AllocationError(Exception):
//|     """Catchall exception for allocation related errors."""
//|
//|     ...
//|
MP_DEFINE_MEMORYMONITOR_EXCEPTION(AllocationError, Exception)

NORETURN void mp_raise_memorymonitor_AllocationError(mp_rom_error_text_t fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_memorymonitor_AllocationError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

static const mp_rom_map_elem_t memorymonitor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_memorymonitor) },
    { MP_ROM_QSTR(MP_QSTR_AllocationAlarm), MP_ROM_PTR(&memorymonitor_allocationalarm_type) },
    { MP_ROM_QSTR(MP_QSTR_AllocationSize), MP_ROM_PTR(&memorymonitor_allocationsize_type) },

    // Errors
    { MP_ROM_QSTR(MP_QSTR_AllocationError),      MP_ROM_PTR(&mp_type_memorymonitor_AllocationError) },
};

static MP_DEFINE_CONST_DICT(memorymonitor_module_globals, memorymonitor_module_globals_table);

void memorymonitor_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_print_kind_t k = kind & ~PRINT_EXC_SUBCLASS;
    bool is_subclass = kind & PRINT_EXC_SUBCLASS;
    if (!is_subclass && (k == PRINT_EXC)) {
        mp_print_str(print, qstr_str(MP_OBJ_QSTR_VALUE(memorymonitor_module_globals_table[0].value)));
        mp_print_str(print, ".");
    }
    mp_obj_exception_print(print, o_in, kind);
}

const mp_obj_module_t memorymonitor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&memorymonitor_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_memorymonitor, memorymonitor_module);
