/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/digitalio/Pull.h"

//| class Pull:
//|     """Defines the pull of a digital input pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the pull value, if any, used while reading
//|         digital values in."""
//|         ...
//|
//|     UP: Pull
//|     """When the input line isn't being driven the pull up can pull the state
//|     of the line high so it reads as true."""
//|
//|     DOWN: Pull
//|     """When the input line isn't being driven the pull down can pull the
//|     state of the line low so it reads as false."""
//|
const mp_obj_type_t digitalio_pull_type;

const digitalio_pull_obj_t digitalio_pull_up_obj = {
    { &digitalio_pull_type },
};

const digitalio_pull_obj_t digitalio_pull_down_obj = {
    { &digitalio_pull_type },
};

STATIC const mp_rom_map_elem_t digitalio_pull_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UP),    MP_ROM_PTR(&digitalio_pull_up_obj) },
    { MP_ROM_QSTR(MP_QSTR_DOWN),  MP_ROM_PTR(&digitalio_pull_down_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_pull_locals_dict, digitalio_pull_locals_dict_table);

STATIC void digitalio_pull_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr pull = MP_QSTR_UP;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_pull_down_obj)) {
        pull = MP_QSTR_DOWN;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_Pull, pull);
}

const mp_obj_type_t digitalio_pull_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pull,
    .print = digitalio_pull_print,
    .locals_dict = (mp_obj_t)&digitalio_pull_locals_dict,
};
