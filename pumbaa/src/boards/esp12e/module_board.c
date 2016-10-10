/**
 * @file module_board.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * Function called when this module is imported.
 */
static mp_obj_t module_init(void)
{
    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_init_obj, module_init);

static const mp_rom_map_elem_t module_board_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_board) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&module_init_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO0), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO2), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO4), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO5), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO12), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO13), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO14), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_PIN_GPIO15), MP_ROM_INT(15) },

    { MP_ROM_QSTR(MP_QSTR_PIN_LED), MP_ROM_INT(2) }
};

static MP_DEFINE_CONST_DICT(module_board_globals, module_board_globals_table);

/**
 * Board module type.
 */
const mp_obj_module_t module_board = {
    { &mp_type_module },
    .globals = (mp_obj_t)&module_board_globals,
};
