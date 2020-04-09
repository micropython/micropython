#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_CLOCK), MP_ROM_PTR(&pin_P0_25) },
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_DATA), MP_ROM_PTR(&pin_P0_28) },

    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_P0_14) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_TFT_SCK), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_PTR(&pin_P0_12) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P1_14) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P1_15) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON_SW1), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_SW2), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_SW3), MP_ROM_PTR(&pin_P0_17) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_SW4), MP_ROM_PTR(&pin_P1_03) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
