#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_B15), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_B14), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_B13), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_B10), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_B1), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA05) },

    { MP_ROM_QSTR(MP_QSTR_B3), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_B4), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_B5), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_B6), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_B7), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA08) },

    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_B9), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_B8), MP_ROM_PTR(&pin_PB08) },

    { MP_ROM_QSTR(MP_QSTR_BTN), MP_ROM_PTR(&pin_PC13) },
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_PB12) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
