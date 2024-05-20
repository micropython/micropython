// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS
    // Default RPi Pico Pins
    {MP_ROM_QSTR(MP_QSTR_GP0), MP_ROM_PTR(&pin_GPIO0)},
    {MP_ROM_QSTR(MP_QSTR_GP1), MP_ROM_PTR(&pin_GPIO1)},
    {MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_PTR(&pin_GPIO2)},
    {MP_ROM_QSTR(MP_QSTR_GP3), MP_ROM_PTR(&pin_GPIO3)},
    {MP_ROM_QSTR(MP_QSTR_GP4), MP_ROM_PTR(&pin_GPIO4)},
    {MP_ROM_QSTR(MP_QSTR_GP5), MP_ROM_PTR(&pin_GPIO5)},
    {MP_ROM_QSTR(MP_QSTR_GP6), MP_ROM_PTR(&pin_GPIO6)},
    {MP_ROM_QSTR(MP_QSTR_GP7), MP_ROM_PTR(&pin_GPIO7)},
    {MP_ROM_QSTR(MP_QSTR_GP8), MP_ROM_PTR(&pin_GPIO8)},
    {MP_ROM_QSTR(MP_QSTR_GP9), MP_ROM_PTR(&pin_GPIO9)},
    {MP_ROM_QSTR(MP_QSTR_GP10), MP_ROM_PTR(&pin_GPIO10)},
    {MP_ROM_QSTR(MP_QSTR_GP11), MP_ROM_PTR(&pin_GPIO11)},
    {MP_ROM_QSTR(MP_QSTR_GP12), MP_ROM_PTR(&pin_GPIO12)},
    {MP_ROM_QSTR(MP_QSTR_GP13), MP_ROM_PTR(&pin_GPIO13)},
    {MP_ROM_QSTR(MP_QSTR_GP14), MP_ROM_PTR(&pin_GPIO14)},
    {MP_ROM_QSTR(MP_QSTR_GP15), MP_ROM_PTR(&pin_GPIO15)},
    {MP_ROM_QSTR(MP_QSTR_GP16), MP_ROM_PTR(&pin_GPIO16)},
    {MP_ROM_QSTR(MP_QSTR_GP17), MP_ROM_PTR(&pin_GPIO17)},
    {MP_ROM_QSTR(MP_QSTR_GP18), MP_ROM_PTR(&pin_GPIO18)},
    {MP_ROM_QSTR(MP_QSTR_GP19), MP_ROM_PTR(&pin_GPIO19)},
    {MP_ROM_QSTR(MP_QSTR_GP20), MP_ROM_PTR(&pin_GPIO20)},
    {MP_ROM_QSTR(MP_QSTR_GP21), MP_ROM_PTR(&pin_GPIO21)},
    {MP_ROM_QSTR(MP_QSTR_GP22), MP_ROM_PTR(&pin_GPIO22)},
    {MP_ROM_QSTR(MP_QSTR_GP26), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_GP27), MP_ROM_PTR(&pin_GPIO27)},
    {MP_ROM_QSTR(MP_QSTR_GP28), MP_ROM_PTR(&pin_GPIO28)},

    // PicoPad external connector
    /*
              ┌──────┐
         GND -│1    2│- GND
         3V3 -│3    4│- VBAT
       D5/A2 -│5    6│- ADC_VREF
       D4/A1 -│7    8│- AGND
   D3/TX/SDA -│9   10│- D0/A0
   D2/RX/SCL -│11  12│- D1
              └──────┘
    */

    {MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO14)},
    {MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO1)},
    {MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO0)},
    {MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO27)},
    {MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO28)},

    {MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27)},
    {MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO28)},

    // PicoPad LCD
    {MP_ROM_QSTR(MP_QSTR_LCD_RESET), MP_ROM_PTR(&pin_GPIO20)},
    {MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO21)},
    {MP_ROM_QSTR(MP_QSTR_LCD_SCLK), MP_ROM_PTR(&pin_GPIO18)},
    {MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO19)},
    {MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO17)},
    {MP_ROM_QSTR(MP_QSTR_LCD_BL), MP_ROM_PTR(&pin_GPIO16)},

    // PicoPad Audio
    {MP_ROM_QSTR(MP_QSTR_AUDIO), MP_ROM_PTR(&pin_GPIO15)},

    // PicoPad USR LED (yellow)
    {MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO22)},

    // PicoPad Buttons
    {MP_ROM_QSTR(MP_QSTR_SW_X), MP_ROM_PTR(&pin_GPIO9)},
    {MP_ROM_QSTR(MP_QSTR_SW_Y), MP_ROM_PTR(&pin_GPIO8)},
    {MP_ROM_QSTR(MP_QSTR_SW_A), MP_ROM_PTR(&pin_GPIO7)},
    {MP_ROM_QSTR(MP_QSTR_SW_B), MP_ROM_PTR(&pin_GPIO6)},

    {MP_ROM_QSTR(MP_QSTR_SW_DOWN), MP_ROM_PTR(&pin_GPIO5)},
    {MP_ROM_QSTR(MP_QSTR_SW_RIGHT), MP_ROM_PTR(&pin_GPIO2)},
    {MP_ROM_QSTR(MP_QSTR_SW_LEFT), MP_ROM_PTR(&pin_GPIO3)},
    {MP_ROM_QSTR(MP_QSTR_SW_UP), MP_ROM_PTR(&pin_GPIO4)},

    // PicoPad SD Card
    {MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO13)},
    {MP_ROM_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_GPIO10)},
    {MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO11)},
    {MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO12)},

    // PicoPad Battery
    {MP_ROM_QSTR(MP_QSTR_BAT_SENSE), MP_ROM_PTR(&pin_GPIO29)},

    // PicoPad UART0
    {MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO0)},
    {MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO1)},

    // PicoPad I2C0
    {MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO0)},
    {MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO1)},

    // PicoPad I2C1
    {MP_ROM_QSTR(MP_QSTR_SDA_ALT), MP_ROM_PTR(&pin_GPIO14)},
    {MP_ROM_QSTR(MP_QSTR_SCL_ALT), MP_ROM_PTR(&pin_GPIO27)},


    {MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj)},
    {MP_ROM_QSTR(MP_QSTR_I2C_ALT), MP_ROM_PTR(&board_i2c_obj)},
    {MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj)},

    {MP_ROM_QSTR(MP_QSTR_SMPS_MODE), MP_ROM_PTR(&pin_CYW1)},
    {MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_CYW0)},
    {MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_CYW2)},
    {MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
