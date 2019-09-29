/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_ESP32_CAN_H
#define MICROPY_INCLUDED_ESP32_CAN_H

#include "modmachine.h"

#include "py/obj.h"

typedef struct _machine_can_obj_t {
    mp_obj_base_t base;
    uint8_t tx;
    uint8_t rx;
} machine_hw_can_obj_t;

//Functions signature definition
STATIC mp_obj_t machine_hw_can_init(mp_obj_t self_in);
STATIC void machine_hw_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

//Python function declarations
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_hw_can_init_obj, machine_hw_can_init);

//Python objects list declaration
STATIC const mp_rom_map_elem_t machine_can_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_hw_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_125k), MP_ROM_INT(125) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_250k), MP_ROM_INT(250) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_500k), MP_ROM_INT(500) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_1M),   MP_ROM_INT(1000) },
};
STATIC MP_DEFINE_CONST_DICT(machine_can_locals_dict, machine_can_locals_dict_table);

//Python object definition
const mp_obj_type_t machine_can_type = {
    { &mp_type_type },
    .name = MP_QSTR_CAN,
    .locals_dict = (mp_obj_dict_t*)&machine_can_locals_dict,
};

#endif // MICROPY_INCLUDED_ESP32_CAN_H
