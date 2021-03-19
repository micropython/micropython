#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_AIN0), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_AIN1), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_AIN2), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_AIN3), MP_ROM_PTR(&pin_P0_05) },

    { MP_ROM_QSTR(MP_QSTR_AREF), MP_ROM_PTR(&pin_P0_04) }, // User must connect manually.
    { MP_ROM_QSTR(MP_QSTR_VDIV), MP_ROM_PTR(&pin_P0_05) }, // User must connect manually.

    { MP_ROM_QSTR(MP_QSTR_NFC1), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_NFC2), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_P8), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_P9), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_P10), MP_ROM_PTR(&pin_P0_10) },
    { MP_ROM_QSTR(MP_QSTR_P18), MP_ROM_PTR(&pin_P0_18) }, // !Reset button.
    { MP_ROM_QSTR(MP_QSTR_P19), MP_ROM_PTR(&pin_P0_19) },
    { MP_ROM_QSTR(MP_QSTR_P22), MP_ROM_PTR(&pin_P0_22) }, // green led, low is on.
    { MP_ROM_QSTR(MP_QSTR_P23), MP_ROM_PTR(&pin_P0_23) }, // red led, low is on.
    { MP_ROM_QSTR(MP_QSTR_P24), MP_ROM_PTR(&pin_P0_24) }, // blue led, low is on.

    { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_P0_23) }, // Low is on.
    { MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_PTR(&pin_P0_22) }, // Low is on.
    { MP_ROM_QSTR(MP_QSTR_LED_BLUE), MP_ROM_PTR(&pin_P0_24) }, // Low is on.

    // BUT this is the RESET pin so we can't really use it.
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_P0_18) }, // Low is pressed.
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
