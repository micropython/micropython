#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // First PMOD connector
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_GPIO9) },

    { MP_ROM_QSTR(MP_QSTR_U0TXD), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_G43), MP_ROM_PTR(&pin_GPIO43) },

    { MP_ROM_QSTR(MP_QSTR_U0RXD), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_G44), MP_ROM_PTR(&pin_GPIO44) },

    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_GPIO10) },

    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_G11), MP_ROM_PTR(&pin_GPIO11) },

    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_G13), MP_ROM_PTR(&pin_GPIO13) },

    { MP_ROM_QSTR(MP_QSTR_CLK), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_G12), MP_ROM_PTR(&pin_GPIO12) },

    { MP_ROM_QSTR(MP_QSTR_G14), MP_ROM_PTR(&pin_GPIO14) },

    // Second PMOD connector
    { MP_ROM_QSTR(MP_QSTR_G38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_G39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_SCL2), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_G40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_SDA2), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_G41), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_G42), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_G21), MP_ROM_PTR(&pin_GPIO21) },

    // LCD & touchscreen
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SCK), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RST), MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CTRL), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_CTP_INT), MP_ROM_PTR(&pin_GPIO3) },

    // Audio
    { MP_ROM_QSTR(MP_QSTR_I2S_ADC_SDOUT), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_I2S_SCLK), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCK), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_I2S_CODEC_DSDIN), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_PA_CTRL), MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_MUTE_STATUS), MP_ROM_PTR(&pin_GPIO1) },

    // Internal I2C bus
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO18) },

    // boot button, also usable as a software button
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_GPIO0) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
