// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_IO0), MP_ROM_PTR(&pin_GPIO0) },

    // Battery voltage sense pin
    { MP_ROM_QSTR(MP_QSTR_VBAT_VOLTAGE), MP_ROM_PTR(&pin_GPIO17) },

    // 5V present sense pin
    { MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO16) },

    // SPI
    { MP_ROM_QSTR(MP_QSTR_MO), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_MI), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_CLK), MP_ROM_PTR(&pin_GPIO36) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO9) },

    // uSD Card
    { MP_ROM_QSTR(MP_QSTR_SD_DETECT), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO21) },

    // User Buttons
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_C), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_D), MP_ROM_PTR(&pin_GPIO34) },

    // I2S Mic ICS-43434
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_DATA), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_BCLK), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_WS), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_SEL), MP_ROM_PTR(&pin_GPIO39) },

    // I2S Amplifier MAX98357A
    { MP_ROM_QSTR(MP_QSTR_I2S_AMP_LRCLK), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_I2S_AMP_BCLK), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_I2S_AMP_DATA), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_I2S_AMP_SD), MP_ROM_PTR(&pin_GPIO4) },

    // RTC Interrupt Pin
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), MP_ROM_PTR(&pin_GPIO7) },

    // LED Matrix
    { MP_ROM_QSTR(MP_QSTR_MATRIX_POWER), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_MATRIX_DATA), MP_ROM_PTR(&pin_GPIO18) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
