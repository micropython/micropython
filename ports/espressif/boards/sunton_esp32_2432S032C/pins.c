// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(lcd_spi, spi, 0)
CIRCUITPY_BOARD_BUS_SINGLETON(sd_spi, spi, 1)

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Boot button
    { MP_ROM_QSTR(MP_QSTR_BOOT0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },

    // RGB LED
    { MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_LED_BLUE), MP_ROM_PTR(&pin_GPIO17) },

    // CDS Light sensor (Not present on all boards)
    { MP_ROM_QSTR(MP_QSTR_LDR), MP_ROM_PTR(&pin_GPIO34) },

    // Speaker pin
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_GPIO26) },

    // User available GPIOs:
    // P3 pin 1 and CN1 pin 2, shared with touch-interrupt and SDA
    { MP_ROM_QSTR(MP_QSTR_IO21), MP_ROM_PTR(&pin_GPIO21) },
    // P3 pin 2 and CN1 pin 3, shared with SCL
    { MP_ROM_QSTR(MP_QSTR_IO22), MP_ROM_PTR(&pin_GPIO22) },
    // P3 pin 3, input only
    { MP_ROM_QSTR(MP_QSTR_IO35), MP_ROM_PTR(&pin_GPIO35) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO22) },

    // TF card slot (SPI)
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO5) },

    // ST7789 (SPI)
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MISO), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SCK), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BCKL), MP_ROM_PTR(&pin_GPIO27) },

    // GT911 (I2C)
    { MP_ROM_QSTR(MP_QSTR_TOUCH_SDA), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_SCL), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_RST), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_INT), MP_ROM_PTR(&pin_GPIO21) },

    // objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SPI), MP_ROM_PTR(&board_lcd_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_SD_SPI), MP_ROM_PTR(&board_sd_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
