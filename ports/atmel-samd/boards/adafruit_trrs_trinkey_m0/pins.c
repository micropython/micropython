// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA01) },

    { MP_ROM_QSTR(MP_QSTR_TIP), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_TIP_SWITCH), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_RING_2), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_SLEEVE), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_RING_1), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_RING_1_SWITCH), MP_ROM_PTR(&pin_PA07) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA08) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
