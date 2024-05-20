// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_OBJ_NEW_QSTR(MP_QSTR_DENSITY), MP_ROM_PTR(&pin_GPIO0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SELECT), MP_ROM_PTR(&pin_GPIO1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOTOR), MP_ROM_PTR(&pin_GPIO2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION), MP_ROM_PTR(&pin_GPIO3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STEP), MP_ROM_PTR(&pin_GPIO4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WRDATA), MP_ROM_PTR(&pin_GPIO5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WRGATE), MP_ROM_PTR(&pin_GPIO6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SIDE), MP_ROM_PTR(&pin_GPIO7) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_FLOPPY_DIRECTION), MP_ROM_PTR(&pin_GPIO8) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INDEX), MP_ROM_PTR(&pin_GPIO9) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TRACK0), MP_ROM_PTR(&pin_GPIO10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WRPROT), MP_ROM_PTR(&pin_GPIO11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RDDATA), MP_ROM_PTR(&pin_GPIO12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_READY), MP_ROM_PTR(&pin_GPIO13) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO17) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO14) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CD), MP_ROM_PTR(&pin_GPIO24) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_GPIO18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO18) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_COMMAND), MP_ROM_PTR(&pin_GPIO19) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_GPIO20) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_GPIO21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_GPIO22) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDIO_DATA3), MP_ROM_PTR(&pin_GPIO23) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
