#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t tft_r_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO14),
        MP_ROM_PTR(&pin_GPIO21),
        MP_ROM_PTR(&pin_GPIO47),
        MP_ROM_PTR(&pin_GPIO48),
        MP_ROM_PTR(&pin_GPIO45),
    }
};

STATIC const mp_rom_obj_tuple_t tft_g_pins = {
    {&mp_type_tuple},
    6,
    {
        MP_ROM_PTR(&pin_GPIO4),
        MP_ROM_PTR(&pin_GPIO16),
        MP_ROM_PTR(&pin_GPIO15),
        MP_ROM_PTR(&pin_GPIO7),
        MP_ROM_PTR(&pin_GPIO6),
        MP_ROM_PTR(&pin_GPIO5),
    }
};

STATIC const mp_rom_obj_tuple_t tft_b_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO1),
        MP_ROM_PTR(&pin_GPIO9),
        MP_ROM_PTR(&pin_GPIO46),
        MP_ROM_PTR(&pin_GPIO3),
        MP_ROM_PTR(&pin_GPIO8),
    }
};

STATIC const mp_rom_map_elem_t tft_table[] = {
    { MP_ROM_QSTR(MP_QSTR_de), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_vsync), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_hsync), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_dclk), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_ROM_PTR(&tft_r_pins) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_ROM_PTR(&tft_g_pins) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_ROM_PTR(&tft_b_pins) },
};
MP_DEFINE_CONST_DICT(tft_dict, tft_table);

STATIC const mp_rom_map_elem_t timings800_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_INT(6500000) }, // nominal 16MHz, but display is unstable/tears at that frequency
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_INT(800) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_INT(480) },
    { MP_ROM_QSTR(MP_QSTR_hsync_pulse_width), MP_ROM_INT(30) },
    { MP_ROM_QSTR(MP_QSTR_hsync_front_porch), MP_ROM_INT(210) },
    { MP_ROM_QSTR(MP_QSTR_hsync_back_porch), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_hsync_idle_low), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_vsync_pulse_width), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_vsync_front_porch), MP_ROM_INT(22) },
    { MP_ROM_QSTR(MP_QSTR_vsync_back_porch), MP_ROM_INT(10) },
    { MP_ROM_QSTR(MP_QSTR_vsync_idle_low), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_de_idle_high), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_pclk_active_high), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_pclk_idle_high), MP_ROM_FALSE },
};
MP_DEFINE_CONST_DICT(timings800_dict, timings800_table);

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TFT), MP_ROM_PTR(&tft_dict) },
    { MP_ROM_QSTR(MP_QSTR_TIMINGS800), MP_ROM_PTR(&timings800_dict) },
    { MP_ROM_QSTR(MP_QSTR_BACKLIGHT), MP_ROM_PTR(&pin_GPIO10) },

    { MP_ROM_QSTR(MP_QSTR_I2S_SCK), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WS), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_I2S_SDO), MP_ROM_PTR(&pin_GPIO19) },

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO17) },

    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO12) },

    // boot mode button can be used in SW as well
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO1) },

    // IO10 <> SD_CS is cut at factory (non-placed resistor position R34) and pulled up.
    // Permanent SDIO 1-bit mode?

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
