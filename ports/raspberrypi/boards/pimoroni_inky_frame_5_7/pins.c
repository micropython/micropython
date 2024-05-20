// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

#include "supervisor/board.h"
#include "shared-module/displayio/__init__.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "inky-shared.h"

// for use with keypad.ShiftRegisterKeys: map keycode (bit-number)
// to logical names board.KEYCODES.SW_A etc.
// N.B.: labels and bit-numbers in the schematic are reversed, i.e.
//       SW_A on D0 has bit-number 7

static const qstr board_keycodes_fields[] = {
    MP_QSTR_SW_A,
    MP_QSTR_SW_B,
    MP_QSTR_SW_C,
    MP_QSTR_SW_D,
    MP_QSTR_SW_E,
    MP_QSTR_RTC_ALARM,
    MP_QSTR_EXT_TRIGGER,
    MP_QSTR_INKY_BUS
};

static MP_DEFINE_ATTRTUPLE(
    board_keycodes_obj,
    board_keycodes_fields,
    8,
    MP_ROM_INT(7),
    MP_ROM_INT(6),
    MP_ROM_INT(5),
    MP_ROM_INT(4),
    MP_ROM_INT(3),
    MP_ROM_INT(2),
    MP_ROM_INT(1),
    MP_ROM_INT(0)
    );

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO5) },

    { MP_ROM_QSTR(MP_QSTR_LED_ACT), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_LED_CONN), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_CLK), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_LATCH), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_OUT), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_LED_A), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LED_C), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_LED_D), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_LED_E), MP_ROM_PTR(&pin_GPIO15) },

    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_INKY_CS), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_SCLK), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT1), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT2), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO22) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },

    { MP_ROM_QSTR(MP_QSTR_INKY_RES), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_INKY_DC), MP_ROM_PTR(&pin_GPIO28) },

    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO29) },

    { MP_ROM_QSTR(MP_QSTR_PICO_LED), MP_ROM_PTR(&pin_CYW0) },
    { MP_ROM_QSTR(MP_QSTR_SMPS_MODE), MP_ROM_PTR(&pin_CYW1) },
    { MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_CYW2) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].epaper_display)},
    { MP_ROM_QSTR(MP_QSTR_ENABLE_DIO), MP_ROM_PTR(&enable_pin_obj)},   // GP2
    { MP_ROM_QSTR(MP_QSTR_KEYCODES), MP_ROM_PTR(&board_keycodes_obj)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
