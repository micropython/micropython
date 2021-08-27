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

#include <stdint.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

//| class Direction:
//|     """Defines the direction of a digital pin"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define which direction the digital values are
//|         going."""
//|         ...
//|
//|     INPUT: Direction
//|     """Read digital data in"""
//|
//|     OUTPUT: Direction
//|     """Write digital data out"""
//|
const mp_obj_type_t digitalio_direction_type;

const digitalio_direction_obj_t digitalio_direction_input_obj = {
    { &digitalio_direction_type },
};

const digitalio_direction_obj_t digitalio_direction_output_obj = {
    { &digitalio_direction_type },
};

STATIC const mp_rom_map_elem_t digitalio_direction_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_INPUT),    MP_ROM_PTR(&digitalio_direction_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_OUTPUT),   MP_ROM_PTR(&digitalio_direction_output_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_direction_locals_dict, digitalio_direction_locals_dict_table);

STATIC void digitalio_direction_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr direction = MP_QSTR_INPUT;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_direction_output_obj)) {
        direction = MP_QSTR_OUTPUT;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_Direction, direction);
}

const mp_obj_type_t digitalio_direction_type = {
    { &mp_type_type },
    .name = MP_QSTR_Direction,
    .print = digitalio_direction_print,
    .locals_dict = (mp_obj_t)&digitalio_direction_locals_dict,
};
