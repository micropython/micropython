#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_A9), MP_ROM_PTR(&pin_PA09) }, 
    { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_A11), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_A12), MP_ROM_PTR(&pin_PA12) },
    { MP_ROM_QSTR(MP_QSTR_A13), MP_ROM_PTR(&pin_PA13) },
    { MP_ROM_QSTR(MP_QSTR_A14), MP_ROM_PTR(&pin_PA14) },

    { MP_ROM_QSTR(MP_QSTR_B4), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_B5), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_B6), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_B7), MP_ROM_PTR(&pin_PB07) },

    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_LED4), MP_ROM_PTR(&pin_PA03) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PB04) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
