/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Radomir Dopieralski
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
#include "py/mphal.h"
#include "PewPew.h"
#include "common-hal/_pew/PewPew.h"


STATIC mp_obj_t get_pressed(void) {
    pew_obj_t *pew = MP_STATE_VM(pew_singleton);
    if (!pew) {
        return mp_const_none;
    }
    uint8_t pressed = pew->pressed;
    pew->pressed = 0;
    return mp_obj_new_int(pressed);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_pressed_obj, get_pressed);


STATIC mp_obj_t get_ticks(void) {
    return mp_obj_new_int(pew_get_ticks());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_ticks_obj, get_ticks);


//| """LED matrix driver"""
//|
STATIC const mp_rom_map_elem_t pew_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pew) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PewPew),  MP_ROM_PTR(&pewpew_type)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pressed),  MP_ROM_PTR(&get_pressed_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_ticks),  MP_ROM_PTR(&get_ticks_obj)},
};
STATIC MP_DEFINE_CONST_DICT(pew_module_globals,
    pew_module_globals_table);

const mp_obj_module_t pew_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pew_module_globals,
};
