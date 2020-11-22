/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#include <stdbool.h>
#include "py/objproperty.h"
#include "shared-bindings/supervisor/Runtime.h"

//TODO: add USB, REPL to description once they're operational
//| class Runtime:
//|     """Current status of runtime objects.
//|
//|     Usage::
//|
//|        import supervisor
//|        if supervisor.runtime.serial_connected:
//|            print("Hello World!")"""
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `supervisor.Runtime`.
//|         Use `supervisor.runtime` to access the sole instance available."""
//|         ...
//|

//|     serial_connected: bool
//|     """Returns the USB serial communication status (read-only)."""
//|

STATIC mp_obj_t supervisor_get_serial_connected(mp_obj_t self){
    if (!common_hal_get_serial_connected()) {
        return mp_const_false;
    }
    else {
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_get_serial_connected_obj, supervisor_get_serial_connected);

const mp_obj_property_t supervisor_serial_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&supervisor_get_serial_connected_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|     serial_bytes_available: int
//|     """Returns the whether any bytes are available to read
//|     on the USB serial input.  Allows for polling to see whether
//|     to call the built-in input() or wait. (read-only)"""
//|
STATIC mp_obj_t supervisor_get_serial_bytes_available(mp_obj_t self){
    if (!common_hal_get_serial_bytes_available()) {
        return mp_const_false;
    }
    else {
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_get_serial_bytes_available_obj, supervisor_get_serial_bytes_available);

const mp_obj_property_t supervisor_serial_bytes_available_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&supervisor_get_serial_bytes_available_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|     run_reason: RunReason
//|     """Returns why CircuitPython started running this particular time.
//|
STATIC mp_obj_t supervisor_get_run_reason(mp_obj_t self) {
    mp_raise_NotImplementedError(NULL);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_get_run_reason_obj, supervisor_get_run_reason);

const mp_obj_property_t supervisor_run_reason_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&supervisor_get_run_reason_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t supervisor_runtime_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_serial_connected), MP_ROM_PTR(&supervisor_serial_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_serial_bytes_available), MP_ROM_PTR(&supervisor_serial_bytes_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_run_reason), MP_ROM_PTR(&supervisor_run_reason_obj) },
};

STATIC MP_DEFINE_CONST_DICT(supervisor_runtime_locals_dict, supervisor_runtime_locals_dict_table);

const mp_obj_type_t supervisor_runtime_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Runtime,
    .locals_dict = (mp_obj_t)&supervisor_runtime_locals_dict,
};
