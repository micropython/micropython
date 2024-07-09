// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    {MP_ROM_QSTR(MP_QSTR_GP0), MP_ROM_PTR(&pin_GPIO0)},
    {MP_ROM_QSTR(MP_QSTR_GP1), MP_ROM_PTR(&pin_GPIO1)},
    {MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_PTR(&pin_GPIO2)},
    {MP_ROM_QSTR(MP_QSTR_BTN1), MP_ROM_PTR(&pin_GPIO3)},
    {MP_ROM_QSTR(MP_QSTR_BTN3), MP_ROM_PTR(&pin_GPIO4)},
    {MP_ROM_QSTR(MP_QSTR_BTN2), MP_ROM_PTR(&pin_GPIO5)},
    {MP_ROM_QSTR(MP_QSTR_M1), MP_ROM_PTR(&pin_GPIO7)},
    {MP_ROM_QSTR(MP_QSTR_M2), MP_ROM_PTR(&pin_GPIO11)},
    {MP_ROM_QSTR(MP_QSTR_M3), MP_ROM_PTR(&pin_GPIO13)},
    {MP_ROM_QSTR(MP_QSTR_M4), MP_ROM_PTR(&pin_GPIO9)},
    {MP_ROM_QSTR(MP_QSTR_M5), MP_ROM_PTR(&pin_GPIO8)},
    {MP_ROM_QSTR(MP_QSTR_M6), MP_ROM_PTR(&pin_GPIO10)},
    {MP_ROM_QSTR(MP_QSTR_M7), MP_ROM_PTR(&pin_GPIO12)},
    {MP_ROM_QSTR(MP_QSTR_M8), MP_ROM_PTR(&pin_GPIO14)},
    {MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO18)},

    {MP_ROM_QSTR(MP_QSTR_SMPS_MODE), MP_ROM_PTR(&pin_GPIO23)},

    {MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO24)},

    {MP_ROM_QSTR(MP_QSTR_GP26_A0), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_GP26), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26)},

    {MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_GPIO29)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
