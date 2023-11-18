#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(cam_i2c, i2c, 1) // Camera sensor

STATIC const mp_rom_obj_tuple_t camera_data_tuple = {
    // The order matters.
    // They must be ordered from low to high (Y2, Y3 .. Y9).

    // Do not include any of the control pins in here.
    {&mp_type_tuple},
    8,
    {
        MP_ROM_PTR(&pin_GPIO39), // Y2
        MP_ROM_PTR(&pin_GPIO40), // Y3
        MP_ROM_PTR(&pin_GPIO41), // Y4
        MP_ROM_PTR(&pin_GPIO4),  // Y5
        MP_ROM_PTR(&pin_GPIO7),  // Y6
        MP_ROM_PTR(&pin_GPIO8),  // Y7
        MP_ROM_PTR(&pin_GPIO46), // Y8
        MP_ROM_PTR(&pin_GPIO48)  // Y9
    }
};


STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Left header, module facing down.
    { MP_ROM_QSTR(MP_QSTR_IO17),     MP_ROM_PTR(&pin_GPIO17)},
    { MP_ROM_QSTR(MP_QSTR_SCK),      MP_ROM_PTR(&pin_GPIO17)},

    { MP_ROM_QSTR(MP_QSTR_IO15),     MP_ROM_PTR(&pin_GPIO15)},
    { MP_ROM_QSTR(MP_QSTR_MOSI),     MP_ROM_PTR(&pin_GPIO15)},

    { MP_ROM_QSTR(MP_QSTR_IO16),     MP_ROM_PTR(&pin_GPIO16)},
    { MP_ROM_QSTR(MP_QSTR_MISO),     MP_ROM_PTR(&pin_GPIO16)},

    { MP_ROM_QSTR(MP_QSTR_SPI),  MP_ROM_PTR(&board_spi_obj) },

    { MP_ROM_QSTR(MP_QSTR_IO2),      MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_SCL),      MP_ROM_PTR(&pin_GPIO2) },

    { MP_ROM_QSTR(MP_QSTR_IO1),      MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_SDA),      MP_ROM_PTR(&pin_GPIO3) },

    // I2C cannot be used when CAM_I2C is in use.
    { MP_ROM_QSTR(MP_QSTR_I2C),  MP_ROM_PTR(&board_i2c_obj) },

    { MP_ROM_QSTR(MP_QSTR_IO0),      MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_D9),       MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BOOT),     MP_ROM_PTR(&pin_GPIO0) },

    { MP_ROM_QSTR(MP_QSTR_IO9),      MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_D7),       MP_ROM_PTR(&pin_GPIO9) },

    { MP_ROM_QSTR(MP_QSTR_IO18),     MP_ROM_PTR(&pin_GPIO18)},
    { MP_ROM_QSTR(MP_QSTR_D6),       MP_ROM_PTR(&pin_GPIO18)},

    { MP_ROM_QSTR(MP_QSTR_IO7),      MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_D5),       MP_ROM_PTR(&pin_GPIO7) },

    { MP_ROM_QSTR(MP_QSTR_IO38),     MP_ROM_PTR(&pin_GPIO38)},
    { MP_ROM_QSTR(MP_QSTR_D3),       MP_ROM_PTR(&pin_GPIO38)},

    { MP_ROM_QSTR(MP_QSTR_IO3),      MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_D2),       MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_IO43),     MP_ROM_PTR(&pin_GPIO43)},
    { MP_ROM_QSTR(MP_QSTR_TXD),      MP_ROM_PTR(&pin_GPIO43)},

    { MP_ROM_QSTR(MP_QSTR_IO44),     MP_ROM_PTR(&pin_GPIO44)},
    { MP_ROM_QSTR(MP_QSTR_RXD),      MP_ROM_PTR(&pin_GPIO44)},

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj)},

    // Right header, module facing down.
    { MP_ROM_QSTR(MP_QSTR_IO47),     MP_ROM_PTR(&pin_GPIO47)},
    { MP_ROM_QSTR(MP_QSTR_D14),      MP_ROM_PTR(&pin_GPIO47)},
    { MP_ROM_QSTR(MP_QSTR_PWR),      MP_ROM_PTR(&pin_GPIO47)},

    { MP_ROM_QSTR(MP_QSTR_IO11),     MP_ROM_PTR(&pin_GPIO11)},
    { MP_ROM_QSTR(MP_QSTR_D11),      MP_ROM_PTR(&pin_GPIO11)},

    { MP_ROM_QSTR(MP_QSTR_IO10),     MP_ROM_PTR(&pin_GPIO10)},
    { MP_ROM_QSTR(MP_QSTR_A4),       MP_ROM_PTR(&pin_GPIO10)},

    { MP_ROM_QSTR(MP_QSTR_IO8),      MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_A3),       MP_ROM_PTR(&pin_GPIO8) },

    { MP_ROM_QSTR(MP_QSTR_IO6),      MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_A2),       MP_ROM_PTR(&pin_GPIO6) },

    { MP_ROM_QSTR(MP_QSTR_IO5),      MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_A1),       MP_ROM_PTR(&pin_GPIO5) },

    { MP_ROM_QSTR(MP_QSTR_IO21),     MP_ROM_PTR(&pin_GPIO21)},
    { MP_ROM_QSTR(MP_QSTR_D13),      MP_ROM_PTR(&pin_GPIO21)},
    { MP_ROM_QSTR(MP_QSTR_LED),      MP_ROM_PTR(&pin_GPIO21)},

    { MP_ROM_QSTR(MP_QSTR_IO12),     MP_ROM_PTR(&pin_GPIO12)},
    { MP_ROM_QSTR(MP_QSTR_D12),      MP_ROM_PTR(&pin_GPIO12)},

    { MP_ROM_QSTR(MP_QSTR_IO13),     MP_ROM_PTR(&pin_GPIO13)},
    { MP_ROM_QSTR(MP_QSTR_IO11),     MP_ROM_PTR(&pin_GPIO13)},

    { MP_ROM_QSTR(MP_QSTR_IO14),     MP_ROM_PTR(&pin_GPIO14)},
    { MP_ROM_QSTR(MP_QSTR_D10),      MP_ROM_PTR(&pin_GPIO14)},

    // Camera data
    { MP_ROM_QSTR(MP_QSTR_CAM_DATA),  MP_ROM_PTR(&camera_data_tuple)},

    { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_CAM_HREF),  MP_ROM_PTR(&pin_GPIO42)},
    { MP_ROM_QSTR(MP_QSTR_CAM_PCLK),  MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_CAM_XCLK),  MP_ROM_PTR(&pin_GPIO45)},

    { MP_ROM_QSTR(MP_QSTR_CAM_I2C),   MP_ROM_PTR(&board_cam_i2c_obj)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
