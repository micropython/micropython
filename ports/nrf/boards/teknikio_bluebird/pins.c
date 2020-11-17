#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {

    { MP_ROM_QSTR(MP_QSTR_P0_12),                   MP_ROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_D9),                      MP_ROM_PTR(&pin_P0_12) },

    { MP_ROM_QSTR(MP_QSTR_P0_11),                   MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_D14),                     MP_ROM_PTR(&pin_P0_11) },

    { MP_ROM_QSTR(MP_QSTR_P1_09),                   MP_ROM_PTR(&pin_P1_09) },
    { MP_ROM_QSTR(MP_QSTR_D8),                      MP_ROM_PTR(&pin_P1_09) },

    { MP_ROM_QSTR(MP_QSTR_P1_08),                   MP_ROM_PTR(&pin_P1_08) },
    { MP_ROM_QSTR(MP_QSTR_D13),                     MP_ROM_PTR(&pin_P1_08) },

    { MP_ROM_QSTR(MP_QSTR_P0_08),                   MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_D12),                     MP_ROM_PTR(&pin_P0_08) },

    { MP_ROM_QSTR(MP_QSTR_P0_07),                   MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_D11),                     MP_ROM_PTR(&pin_P0_07) },

    { MP_ROM_QSTR(MP_QSTR_P0_06),                   MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_D7),                      MP_ROM_PTR(&pin_P0_06) },

    { MP_ROM_QSTR(MP_QSTR_P0_05),                   MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_D5),                      MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_A5),                      MP_ROM_PTR(&pin_P0_05) },

    { MP_ROM_QSTR(MP_QSTR_P0_04),                   MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_D6),                      MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_A6),                      MP_ROM_PTR(&pin_P0_04) },

    { MP_ROM_QSTR(MP_QSTR_P0_29),                   MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_A1),                      MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_D1),                      MP_ROM_PTR(&pin_P0_29) },

    { MP_ROM_QSTR(MP_QSTR_P0_31),                   MP_ROM_PTR(&pin_P0_31) },
    { MP_ROM_QSTR(MP_QSTR_A2),                      MP_ROM_PTR(&pin_P0_31) },
    { MP_ROM_QSTR(MP_QSTR_D2),                      MP_ROM_PTR(&pin_P0_31) },

    { MP_ROM_QSTR(MP_QSTR_P0_02),                   MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_A3),                      MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_D3),                      MP_ROM_PTR(&pin_P0_02) },

    { MP_ROM_QSTR(MP_QSTR_P0_03),                   MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_A4),                      MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_D4),                      MP_ROM_PTR(&pin_P0_03) },

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL),                MP_ROM_PTR(&pin_P1_15) },

    { MP_ROM_QSTR(MP_QSTR_LIGHT),                   MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_LIGHT_ENABLE),            MP_ROM_PTR(&pin_P0_30) },

    { MP_ROM_QSTR(MP_QSTR_SPEAKER),                 MP_ROM_PTR(&pin_P1_14) },

    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT), MP_ROM_PTR(&pin_P1_10) },
    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_SDA),       MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_SCL),       MP_ROM_PTR(&pin_P1_11) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
