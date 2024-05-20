// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "PewPew.h"
#include "common-hal/_pew/PewPew.h"


static mp_obj_t get_pressed(void) {
    pew_obj_t *pew = MP_STATE_VM(pew_singleton);
    if (!pew) {
        return mp_const_none;
    }
    uint8_t pressed = pew->pressed;
    pew->pressed = 0;
    return mp_obj_new_int(pressed);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_pressed_obj, get_pressed);


static mp_obj_t get_ticks(void) {
    return mp_obj_new_int(pew_get_ticks());
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_ticks_obj, get_ticks);


//| """LED matrix driver"""
static const mp_rom_map_elem_t pew_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pew) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PewPew),  MP_ROM_PTR(&pewpew_type)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pressed),  MP_ROM_PTR(&get_pressed_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_ticks),  MP_ROM_PTR(&get_ticks_obj)},
};
static MP_DEFINE_CONST_DICT(pew_module_globals,
    pew_module_globals_table);

const mp_obj_module_t pew_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pew_module_globals,
};

MP_REGISTER_ROOT_POINTER(mp_obj_t pew_singleton);
