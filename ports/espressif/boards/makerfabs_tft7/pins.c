// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

static const mp_rom_obj_tuple_t tft_r_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO45),
        MP_ROM_PTR(&pin_GPIO48),
        MP_ROM_PTR(&pin_GPIO47),
        MP_ROM_PTR(&pin_GPIO21),
        MP_ROM_PTR(&pin_GPIO14),
    }
};

static const mp_rom_obj_tuple_t tft_g_pins = {
    {&mp_type_tuple},
    6,
    {
        MP_ROM_PTR(&pin_GPIO5),
        MP_ROM_PTR(&pin_GPIO6),
        MP_ROM_PTR(&pin_GPIO7),
        MP_ROM_PTR(&pin_GPIO15),
        MP_ROM_PTR(&pin_GPIO16),
        MP_ROM_PTR(&pin_GPIO4),
    }
};

static const mp_rom_obj_tuple_t tft_b_pins = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_GPIO8),
        MP_ROM_PTR(&pin_GPIO3),
        MP_ROM_PTR(&pin_GPIO46),
        MP_ROM_PTR(&pin_GPIO9),
        MP_ROM_PTR(&pin_GPIO1),
    }
};

static const mp_rom_map_elem_t tft_pins_table[] = {
    { MP_ROM_QSTR(MP_QSTR_de), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_vsync), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_hsync), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_dclk), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_ROM_PTR(&tft_r_pins) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_ROM_PTR(&tft_g_pins) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_ROM_PTR(&tft_b_pins) },
};
MP_DEFINE_CONST_DICT(tft_pins_dict, tft_pins_table);

static const mp_rom_map_elem_t timings800_table[] = {
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

static const mp_rom_map_elem_t timings1024_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_INT(10000000) }, // nominal 16MHz, but display is unstable/tears at that frequency
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_INT(1024) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_INT(600) },
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
MP_DEFINE_CONST_DICT(timings1024_dict, timings1024_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_TFT_PINS), MP_ROM_PTR(&tft_pins_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS), MP_ROM_PTR(&timings800_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS800), MP_ROM_PTR(&timings800_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS1024), MP_ROM_PTR(&timings1024_dict) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO10) },

    // GPIO pins available on Mabee connector port (also shared with I2S & USB D+/D-)
    { MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_PTR(&pin_GPIO19) },

    // I2S pins are shared with USB D+/D-, these are only useful if USB is disabled
    #if CIRCUITPY_USB_DEVICE == 0
    { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DATA), MP_ROM_PTR(&pin_GPIO19) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_RESET), MP_ROM_PTR(&pin_GPIO38) },

    // IO10 <> SD_CS is cut at factory (non-placed resistor position R34) and pulled up.
    // Permanent SDIO 1-bit mode?
    // Until SDIO 1-bit mode is support on Espressif ports these pins aren't useful
    // { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO11) },
    // { MP_ROM_QSTR(MP_QSTR_SDIO_D0), MP_ROM_PTR(&pin_GPIO13) },
    // { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO12) },

    // boot mode button can be used in SW as well
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO1) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
