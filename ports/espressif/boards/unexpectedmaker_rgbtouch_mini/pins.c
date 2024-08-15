// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit
// Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    {MP_ROM_QSTR(MP_QSTR_IO7), MP_ROM_PTR(&pin_GPIO7)},
    {MP_ROM_QSTR(MP_QSTR_INT_IMU), MP_ROM_PTR(&pin_GPIO7)},

    {MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO8)},
    {MP_ROM_QSTR(MP_QSTR_IO8), MP_ROM_PTR(&pin_GPIO8)},

    {MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO9)},
    {MP_ROM_QSTR(MP_QSTR_IO9), MP_ROM_PTR(&pin_GPIO9)},

    {MP_ROM_QSTR(MP_QSTR_IO15), MP_ROM_PTR(&pin_GPIO15)},
    {MP_ROM_QSTR(MP_QSTR_INT_ROW), MP_ROM_PTR(&pin_GPIO15)},

    {MP_ROM_QSTR(MP_QSTR_IO16), MP_ROM_PTR(&pin_GPIO16)},
    {MP_ROM_QSTR(MP_QSTR_INT_COL), MP_ROM_PTR(&pin_GPIO16)},

    {MP_ROM_QSTR(MP_QSTR_IO18), MP_ROM_PTR(&pin_GPIO18)},
    {MP_ROM_QSTR(MP_QSTR_VBAT_SENSE), MP_ROM_PTR(&pin_GPIO18)},

    {MP_ROM_QSTR(MP_QSTR_IO21), MP_ROM_PTR(&pin_GPIO21)},
    {MP_ROM_QSTR(MP_QSTR_PWR_SHUTDOWN), MP_ROM_PTR(&pin_GPIO21)},

    {MP_ROM_QSTR(MP_QSTR_IO34), MP_ROM_PTR(&pin_GPIO34)},
    {MP_ROM_QSTR(MP_QSTR_AMP_SD), MP_ROM_PTR(&pin_GPIO34)},

    {MP_ROM_QSTR(MP_QSTR_IO35), MP_ROM_PTR(&pin_GPIO35)},
    {MP_ROM_QSTR(MP_QSTR_AMP_DATA), MP_ROM_PTR(&pin_GPIO35)},

    {MP_ROM_QSTR(MP_QSTR_IO36), MP_ROM_PTR(&pin_GPIO36)},
    {MP_ROM_QSTR(MP_QSTR_AMP_BCLK), MP_ROM_PTR(&pin_GPIO36)},

    {MP_ROM_QSTR(MP_QSTR_IO37), MP_ROM_PTR(&pin_GPIO37)},
    {MP_ROM_QSTR(MP_QSTR_AMP_LRCLK), MP_ROM_PTR(&pin_GPIO37)},

    {MP_ROM_QSTR(MP_QSTR_IO48), MP_ROM_PTR(&pin_GPIO48)},
    {MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO48)},

    {MP_ROM_QSTR(MP_QSTR_MATRIX_POWER), MP_ROM_PTR(&pin_GPIO38)},
    {MP_ROM_QSTR(MP_QSTR_MATRIX_DATA), MP_ROM_PTR(&pin_GPIO39)},

    {MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
