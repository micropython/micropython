// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cade Roux
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(epd_spi, spi, 0)

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Buttons
    { MP_ROM_QSTR(MP_QSTR_BUTTON_BOOT), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_EXIT), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_MENU), MP_ROM_PTR(&pin_GPIO2) },

    // Rocker Switch
    { MP_ROM_QSTR(MP_QSTR_ROCKER_DOWN), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_ROCKER_CLICK), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_ROCKER_UP), MP_ROM_PTR(&pin_GPIO6) },

    // LED
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO19) },

    // Device Control (Pins that, when activated, enable devices)
    { MP_ROM_QSTR(MP_QSTR_EPD_ENABLE), MP_ROM_PTR(&pin_GPIO7) },

    // EPD
    { MP_ROM_QSTR(MP_QSTR_EPD_BUSY), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_EPD_RES), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_EPD_DC), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_EPD_CS), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_EPD_CLK), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_EPD_MOSI), MP_ROM_PTR(&pin_GPIO11) },

    // User accessible GPIO
    { MP_ROM_QSTR(MP_QSTR_IO40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_IO41), MP_ROM_PTR(&pin_GPIO41) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    // Objects
    { MP_ROM_QSTR(MP_QSTR_EPD_SPI), MP_ROM_PTR(&board_epd_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
