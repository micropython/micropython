// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Chris Drake, independently providing these changes.
//
// SPDX-License-Identifier: MIT

#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(sscb_i2c, i2c, 2) // Camera sensor

static const mp_rom_obj_tuple_t camera_data_tuple = {
    // The order matters. They must be ordered from low to high (Y2, Y3 .. Y9).  Do not include any of the control pins in here.
    {&mp_type_tuple},
    8,
    {
        MP_ROM_PTR(&pin_GPIO5),  // Y2
        MP_ROM_PTR(&pin_GPIO18), // Y3
        MP_ROM_PTR(&pin_GPIO19), // Y4
        MP_ROM_PTR(&pin_GPIO21), // Y5
        MP_ROM_PTR(&pin_GPIO36), // Y6
        MP_ROM_PTR(&pin_GPIO39), // Y7
        MP_ROM_PTR(&pin_GPIO34), // Y8
        MP_ROM_PTR(&pin_GPIO35)  // Y9
    }
};

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Red LED on the back of the board
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_LED_INVERTED), MP_ROM_PTR(&pin_GPIO33) },

    // Bright white LED flashlight on the front (be aware that the SD card uses this when in fast mode)
    { MP_ROM_QSTR(MP_QSTR_FLASHLIGHT), MP_ROM_PTR(&pin_GPIO4) },

    // The second ESP32-CAM-MB button (first is RST)
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },

    // SD Card (fast mode aka SDMMC)
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO14)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO15)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_D0),  MP_ROM_PTR(&pin_GPIO2)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_D1),  MP_ROM_PTR(&pin_GPIO4)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_D2),  MP_ROM_PTR(&pin_GPIO12)},
    { MP_ROM_QSTR(MP_QSTR_SDIO_D3),  MP_ROM_PTR(&pin_GPIO13)},

    // SD Card (slow mode)
    { MP_ROM_QSTR(MP_QSTR_SD_SCK),  MP_ROM_PTR(&pin_GPIO14)},
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO15)},
    { MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO2)},
    { MP_ROM_QSTR(MP_QSTR_SD_CS),   MP_ROM_PTR(&pin_GPIO13)},
    { MP_ROM_QSTR(MP_QSTR_SD_SPI),  MP_ROM_PTR(&board_spi_obj) },

    // Camera data
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA),  MP_ROM_PTR(&camera_data_tuple) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA2), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA3), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA4), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA5), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA6), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA7), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA8), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA9), MP_ROM_PTR(&pin_GPIO35) },

    { MP_ROM_QSTR(MP_QSTR_CAMERA_HREF),  MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_PCLK),  MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_PWDN),  MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_RESET), NULL },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_VSYNC), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_XCLK),  MP_ROM_PTR(&pin_GPIO0) },

    { MP_ROM_QSTR(MP_QSTR_CAMERA_SIOD),  MP_ROM_PTR(&pin_GPIO26) }, // SDA
    { MP_ROM_QSTR(MP_QSTR_CAMERA_SIOC),  MP_ROM_PTR(&pin_GPIO27) }, // SCL

    { MP_ROM_QSTR(MP_QSTR_U0R), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_U0T), MP_ROM_PTR(&pin_GPIO1) },

    { MP_ROM_QSTR(MP_QSTR_IO0),          MP_ROM_PTR(&pin_GPIO0) }, // the button
    { MP_ROM_QSTR(MP_QSTR_IO1),          MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_IO3),          MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_IO16),         MP_ROM_PTR(&pin_GPIO16) },

    { MP_ROM_QSTR(MP_QSTR_IO2),          MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_IO4),          MP_ROM_PTR(&pin_GPIO4) },

    { MP_ROM_QSTR(MP_QSTR_IO12),         MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_IO13),         MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_IO14),         MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_IO15),         MP_ROM_PTR(&pin_GPIO15) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) }

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
