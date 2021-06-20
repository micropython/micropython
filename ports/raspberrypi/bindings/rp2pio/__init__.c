/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/rp2pio/StateMachine.h"
#include "bindings/rp2pio/__init__.h"

//| """Hardware interface to RP2 series' programmable IO (PIO) peripheral."""
//|

//| def pins_are_sequential(pins: List[microcontroller.Pin]) -> bool:
//|     """Return True if the pins have sequential GPIO numbers, False otherwise"""
//|     ...
//|
STATIC mp_obj_t rp2pio_pins_are_sequential(const mp_obj_t pins) {
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(pins, &len, &items);
    return mp_obj_new_bool(common_hal_rp2pio_pins_are_sequential(len, items));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_pins_are_sequential_obj, rp2pio_pins_are_sequential);

STATIC const mp_rom_map_elem_t rp2pio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rp2pio) },
    { MP_ROM_QSTR(MP_QSTR_StateMachine),  MP_ROM_PTR(&rp2pio_statemachine_type) },
    { MP_ROM_QSTR(MP_QSTR_pins_are_sequential),  MP_ROM_PTR(&rp2pio_pins_are_sequential_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rp2pio_module_globals, rp2pio_module_globals_table);

const mp_obj_module_t rp2pio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rp2pio_module_globals,
};
