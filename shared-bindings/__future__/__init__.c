/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
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
//| """In CPython, ``from __future import annotations``
//| indicates that evaluation of annotations is postponed, as described in PEP 563.
//| CircuitPython (and MicroPython) ignore annotations entirely, whether or not this feature is imported.
//| This is a limitation of CircuitPython and MicroPython for efficiency reasons.
//| """

STATIC const mp_rom_map_elem_t future_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR___future__) },

    { MP_ROM_QSTR(MP_QSTR_annotations), mp_const_true },
};

STATIC MP_DEFINE_CONST_DICT(future_module_globals, future_module_globals_table);

const mp_obj_module_t future_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&future_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR___future__, future_module, CIRCUITPY_FUTURE);
