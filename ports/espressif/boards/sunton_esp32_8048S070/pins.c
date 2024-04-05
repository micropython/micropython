#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

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
        MP_ROM_PTR(&pin_GPIO9),
        MP_ROM_PTR(&pin_GPIO46),
        MP_ROM_PTR(&pin_GPIO3),
        MP_ROM_PTR(&pin_GPIO8),
        MP_ROM_PTR(&pin_GPIO16),
        MP_ROM_PTR(&pin_GPIO1),
    }
};

STATIC const mp_rom_obj_tuple_t tft_b_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO15),
        MP_ROM_PTR(&pin_GPIO7),
        MP_ROM_PTR(&pin_GPIO6),
        MP_ROM_PTR(&pin_GPIO5),
        MP_ROM_PTR(&pin_GPIO4),
    }
};

STATIC const mp_rom_map_elem_t tft_pins_table[] = {
    { MP_ROM_QSTR(MP_QSTR_de), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_vsync), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_hsync), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_dclk), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_ROM_PTR(&tft_r_pins) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_ROM_PTR(&tft_g_pins) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_ROM_PTR(&tft_b_pins) },
};
MP_DEFINE_CONST_DICT(tft_pins_dict, tft_pins_table);

STATIC const mp_rom_map_elem_t timings_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_INT(12500000) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_INT(800) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_INT(480) },
    { MP_ROM_QSTR(MP_QSTR_hsync_pulse_width), MP_ROM_INT(30) },
    { MP_ROM_QSTR(MP_QSTR_hsync_back_porch), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_hsync_front_porch), MP_ROM_INT(210) },
    { MP_ROM_QSTR(MP_QSTR_hsync_idle_low), MP_ROM_TRUE },
    { MP_ROM_QSTR(MP_QSTR_vsync_pulse_width), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_vsync_back_porch), MP_ROM_INT(10) },
    { MP_ROM_QSTR(MP_QSTR_vsync_front_porch), MP_ROM_INT(22) },
    { MP_ROM_QSTR(MP_QSTR_vsync_idle_low), MP_ROM_TRUE },
    { MP_ROM_QSTR(MP_QSTR_de_idle_high), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_pclk_active_high), MP_ROM_FALSE },
    { MP_ROM_QSTR(MP_QSTR_pclk_idle_high), MP_ROM_FALSE },

};
MP_DEFINE_CONST_DICT(timings_dict, timings_table);

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Display constructs
    { MP_ROM_QSTR(MP_QSTR_TFT_PINS), MP_ROM_PTR(&tft_pins_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS), MP_ROM_PTR(&timings_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO2) },

    // User buttons
    { MP_ROM_QSTR(MP_QSTR_IO0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BOOT0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },

    // User accessible GPIO
    { MP_ROM_QSTR(MP_QSTR_IO19), MP_ROM_PTR(&pin_GPIO19) }, // P2, External SPI plug
    { MP_ROM_QSTR(MP_QSTR_IO11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_IO12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_IO13), MP_ROM_PTR(&pin_GPIO13) },

    { MP_ROM_QSTR(MP_QSTR_IO20), MP_ROM_PTR(&pin_GPIO20) }, // P3 has 19&20 for I2C bus, plus 17&18
    { MP_ROM_QSTR(MP_QSTR_IO17), MP_ROM_PTR(&pin_GPIO17) }, // P4 & P5 are both 17,18,3.3v,G
    { MP_ROM_QSTR(MP_QSTR_IO18), MP_ROM_PTR(&pin_GPIO18) },

    // UART pins
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO20) },

    // SPI
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO13) },

    // i2s amplifier
    { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DATA), MP_ROM_PTR(&pin_GPIO17) },

    // Touch (GT911 I2C, XPT2046 SPI)
    // There are two versions of this tablet, one with capacitive touch
    // one with resistive. They use their respective bus as defined,
    // with GPIO38 being reset on capacitive and cs on resistive.
    { MP_ROM_QSTR(MP_QSTR_TOUCH_RESET), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_CS), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_INT), MP_ROM_PTR(&pin_GPIO18) },

    // SD Slot (SPI)
    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO10) },

    // Objects
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
