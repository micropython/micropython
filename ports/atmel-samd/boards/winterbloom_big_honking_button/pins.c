// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_HONK_OUT), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_GATE_OUT), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_GATE_IN), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_PITCH_IN), MP_ROM_PTR(&pin_PB08) },
    /* Board revisions starting from v5 have PB10 tied to ground. */
    { MP_ROM_QSTR(MP_QSTR_V5), MP_ROM_PTR(&pin_PB10) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
