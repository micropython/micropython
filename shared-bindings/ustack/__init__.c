// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/ustack/__init__.h"




//| """Stack information and analysis"""
//|

#if MICROPY_MAX_STACK_USAGE
//| def max_stack_usage() -> int:
//|     """Return the maximum excursion of the stack so far."""
//|     ...
//|
static mp_obj_t max_stack_usage(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_max_stack_usage());
}
static MP_DEFINE_CONST_FUN_OBJ_0(max_stack_usage_obj, max_stack_usage);

#endif // MICROPY_MAX_STACK_USAGE

//| def stack_size() -> int:
//|     """Return the size of the entire stack.
//|     Same as in micropython.mem_info(), but returns a value instead
//|     of just printing it."""
//|     ...
//|
static mp_obj_t stack_size(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_stack_size());
}
static MP_DEFINE_CONST_FUN_OBJ_0(stack_size_obj, stack_size);

//| def stack_usage() -> int:
//|     """Return how much stack is currently in use.
//|     Same as micropython.stack_use(); duplicated here for convenience."""
//|     ...
//|
static mp_obj_t stack_usage(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_stack_usage());
}
static MP_DEFINE_CONST_FUN_OBJ_0(stack_usage_obj, stack_usage);

static const mp_rom_map_elem_t ustack_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ustack) },
    #if MICROPY_MAX_STACK_USAGE
    { MP_ROM_QSTR(MP_QSTR_max_stack_usage), MP_ROM_PTR(&max_stack_usage_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_stack_size), MP_ROM_PTR(&stack_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_stack_usage), MP_ROM_PTR(&stack_usage_obj) },
};

static MP_DEFINE_CONST_DICT(ustack_module_globals, ustack_module_globals_table);

const mp_obj_module_t ustack_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ustack_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ustack, ustack_module);
