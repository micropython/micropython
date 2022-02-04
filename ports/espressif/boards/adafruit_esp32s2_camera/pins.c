#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_obj_tuple_t camera_data_tuple = {
    {&mp_type_tuple},
    8,
    {
        MP_ROM_PTR(&pin_GPIO13),
        MP_ROM_PTR(&pin_GPIO15),
        MP_ROM_PTR(&pin_GPIO16),
        MP_ROM_PTR(&pin_GPIO14),
        MP_ROM_PTR(&pin_GPIO12),
        MP_ROM_PTR(&pin_GPIO10),
        MP_ROM_PTR(&pin_GPIO9),
        MP_ROM_PTR(&pin_GPIO7),
    }
};


STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_TFT_SCK), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_GPIO41) },

    { MP_ROM_QSTR(MP_QSTR_CARD_CS), MP_ROM_PTR(&pin_GPIO4) },

    { MP_ROM_QSTR(MP_QSTR_IRQ), MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO21) },

    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_GPIO45) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO2) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO18) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO34) },

    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA), MP_ROM_PTR(&camera_data_tuple) },

    { MP_ROM_QSTR(MP_QSTR_CAMERA_VSYNC), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_HREF), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA9), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_XCLK), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA8), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA7), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_PCLK), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA6), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA2), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA5), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA3), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_CAMERA_DATA4), MP_ROM_PTR(&pin_GPIO16) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
