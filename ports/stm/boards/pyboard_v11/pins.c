#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_Y1), MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_Y2), MP_ROM_PTR(&pin_PC07) },
    { MP_ROM_QSTR(MP_QSTR_Y3), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_Y4), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_Y5), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_Y6), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_Y7), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_Y8), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_Y9), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_Y10), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_Y11), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_Y12), MP_ROM_PTR(&pin_PB01) },

    { MP_ROM_QSTR(MP_QSTR_X1), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_X2), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_X3), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_X4), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_X5), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_X6), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_X7), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_X8), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_X9), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_X10), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_X11), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_X12), MP_ROM_PTR(&pin_PC05) },

    { MP_ROM_QSTR(MP_QSTR_X17), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_X18), MP_ROM_PTR(&pin_PC13) },
    { MP_ROM_QSTR(MP_QSTR_X19), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_X20), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_X21), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_X22), MP_ROM_PTR(&pin_PC03) },

    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_PA13) },

    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_LED4), MP_ROM_PTR(&pin_PA13) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
