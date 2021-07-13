/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
STATIC mp_obj_t max_stack_usage(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_max_stack_usage());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(max_stack_usage_obj, max_stack_usage);

#endif // MICROPY_MAX_STACK_USAGE

//| def stack_size() -> int:
//|     """Return the size of the entire stack.
//|     Same as in micropython.mem_info(), but returns a value instead
//|     of just printing it."""
//|     ...
//|
STATIC mp_obj_t stack_size(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_stack_size());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(stack_size_obj, stack_size);

//| def stack_usage() -> int:
//|     """Return how much stack is currently in use.
//|     Same as micropython.stack_use(); duplicated here for convenience."""
//|     ...
//|
STATIC mp_obj_t stack_usage(void) {
    return MP_OBJ_NEW_SMALL_INT(shared_module_ustack_stack_usage());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(stack_usage_obj, stack_usage);

STATIC const mp_rom_map_elem_t ustack_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ustack) },
    #if MICROPY_MAX_STACK_USAGE
    { MP_ROM_QSTR(MP_QSTR_max_stack_usage), MP_ROM_PTR(&max_stack_usage_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_stack_size), MP_ROM_PTR(&stack_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_stack_usage), MP_ROM_PTR(&stack_usage_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ustack_module_globals, ustack_module_globals_table);

const mp_obj_module_t ustack_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ustack_module_globals,
};
