// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries, 2020 Radomir
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_RST), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_DC), MP_ROM_PTR(&pin_PA19) },

    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_O), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_X), MP_ROM_PTR(&pin_PA15) },

    { MP_ROM_QSTR(MP_QSTR_P1), MP_ROM_PTR(&pin_PA30) },
    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_PA31) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_PA04) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
