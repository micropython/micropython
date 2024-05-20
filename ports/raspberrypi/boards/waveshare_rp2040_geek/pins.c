// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(sd_spi, spi, 1)
static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // 2-3 DEBUG
    { MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_GP3), MP_ROM_PTR(&pin_GPIO3) },
    // 4-5 UART
    { MP_ROM_QSTR(MP_QSTR_GP4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_GP5), MP_ROM_PTR(&pin_GPIO5) },
    // 8-12 LCD
    { MP_ROM_QSTR(MP_QSTR_GP8), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_GP9), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_GP10), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_GP11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_GP12), MP_ROM_PTR(&pin_GPIO12) },
    // 16-17 I2C
    { MP_ROM_QSTR(MP_QSTR_GP16), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_GP17), MP_ROM_PTR(&pin_GPIO17) },
    // 18-23 SD Card
    { MP_ROM_QSTR(MP_QSTR_GP18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_GP19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_GP20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_GP21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_GP22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_GP23), MP_ROM_PTR(&pin_GPIO23) },
    // 25 LCD Backlight
    { MP_ROM_QSTR(MP_QSTR_GP25), MP_ROM_PTR(&pin_GPIO25) },
    // 28-29 I2C
    { MP_ROM_QSTR(MP_QSTR_GP28), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_GP29), MP_ROM_PTR(&pin_GPIO29) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // SPI SD Card
    { MP_ROM_QSTR(MP_QSTR_SD_SCK),  MP_ROM_PTR(&pin_GPIO18)},
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO19)},
    { MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO20)},
    { MP_ROM_QSTR(MP_QSTR_SD_CS),   MP_ROM_PTR(&pin_GPIO23)},
    { MP_ROM_QSTR(MP_QSTR_SD_SPI), MP_ROM_PTR(&board_sd_spi_obj) },

    // SDIO SD Card
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK),  MP_ROM_PTR(&pin_GPIO18)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_COMMAND), MP_ROM_PTR(&pin_GPIO19)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_GPIO20)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_GPIO21)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_GPIO22)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA3),   MP_ROM_PTR(&pin_GPIO23)},

    // LCD
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CLK), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RST), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BACKLIGHT), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
