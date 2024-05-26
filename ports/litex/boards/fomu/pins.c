// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TOUCH1), MP_ROM_PTR(&pin_TOUCH1) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH2), MP_ROM_PTR(&pin_TOUCH2) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH3), MP_ROM_PTR(&pin_TOUCH3) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH4), MP_ROM_PTR(&pin_TOUCH4) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
