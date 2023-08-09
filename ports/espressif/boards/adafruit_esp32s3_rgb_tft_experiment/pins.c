#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t tft_r_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO11),
        MP_ROM_PTR(&pin_GPIO10),
        MP_ROM_PTR(&pin_GPIO9),
        MP_ROM_PTR(&pin_GPIO46),
        MP_ROM_PTR(&pin_GPIO3),
    }
};

STATIC const mp_rom_obj_tuple_t tft_g_pins = {
    {&mp_type_tuple},
    6,
    {
        MP_ROM_PTR(&pin_GPIO48),
        MP_ROM_PTR(&pin_GPIO47),
        MP_ROM_PTR(&pin_GPIO21),
        MP_ROM_PTR(&pin_GPIO14),
        MP_ROM_PTR(&pin_GPIO13),
        MP_ROM_PTR(&pin_GPIO12),
    }
};

STATIC const mp_rom_obj_tuple_t tft_b_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO40),
        MP_ROM_PTR(&pin_GPIO39),
        MP_ROM_PTR(&pin_GPIO38),
        MP_ROM_PTR(&pin_GPIO0),
        MP_ROM_PTR(&pin_GPIO45),
    }
};

STATIC const mp_rom_map_elem_t tft_table[] = {
    { MP_ROM_QSTR(MP_QSTR_de), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_vsync), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_hsync), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_dclk), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_ROM_PTR(&tft_r_pins) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_ROM_PTR(&tft_g_pins) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_ROM_PTR(&tft_b_pins) },
};
MP_DEFINE_CONST_DICT(tft_dict, tft_table);

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TFT), MP_ROM_PTR(&tft_dict) },

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(MICROPY_HW_NEOPIXEL) },

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(DEFAULT_I2C_BUS_SDA) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(DEFAULT_I2C_BUS_SCL) },

    // I/O expander pin numbers
    { MP_ROM_QSTR(MP_QSTR_TFT_SCK), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_TP_IRQ), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_BACKLIGHT), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_BTN_UP), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_BTN_DN), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_INT(7) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
