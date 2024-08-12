// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bill Sideris, independently providing these changes
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(tft_spi, spi, 0)
CIRCUITPY_BOARD_BUS_SINGLETON(radio_spi, spi, 1)
CIRCUITPY_BOARD_BUS_SINGLETON(touch_i2c, i2c, 1)

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // DISPLAY
    { MP_ROM_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_TFT_SCLK), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BL), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_TFT_SPI), MP_ROM_PTR(&board_tft_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},

    // TOUCH
    { MP_ROM_QSTR(MP_QSTR_TOUCH_INT), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_SCL), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_SDA), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_I2C), MP_ROM_PTR(&board_touch_i2c_obj) },

    // RTC
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), MP_ROM_PTR(&pin_GPIO17) },

    // IR
    { MP_ROM_QSTR(MP_QSTR_IR_LED), MP_ROM_PTR(&pin_GPIO2) },

    // AXP2101 PMU
    { MP_ROM_QSTR(MP_QSTR_PMU_INT), MP_ROM_PTR(&pin_GPIO21) },

    // RADIO
    { MP_ROM_QSTR(MP_QSTR_RADIO_SCK), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_MISO), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_MOSI), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_SS), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_DIO1), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_RST), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_BUSY), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_SPI), MP_ROM_PTR(&board_radio_spi_obj) },

    // MIC
    { MP_ROM_QSTR(MP_QSTR_MIC_DATA), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_MIC_SCLK), MP_ROM_PTR(&pin_GPIO44) },

    // MAX98357A SPK
    { MP_ROM_QSTR(MP_QSTR_I2S_BCK), MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WS), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT), MP_ROM_PTR(&pin_GPIO46) },

    // BMA423 AXIS SENSOR
    { MP_ROM_QSTR(MP_QSTR_AXIS_INT), MP_ROM_PTR(&pin_GPIO14) },

    // I2C, AXP2101, BMA423, RTC, DRV2605
    { MP_ROM_QSTR(MP_QSTR_I2C_SCL), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_I2C_SDA), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // MISC
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_GPIO0) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
