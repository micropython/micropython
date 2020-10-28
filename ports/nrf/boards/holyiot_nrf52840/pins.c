#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {

    { MP_ROM_QSTR(MP_QSTR_P1_10), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_P1_11), MP_ROM_PTR(&pin_P1_11) },
    { MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_P1_15), MP_ROM_PTR(&pin_P1_15) },
    { MP_ROM_QSTR(MP_QSTR_P0_03), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_P0_02), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_P0_28), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_P0_29), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_P0_30), MP_ROM_PTR(&pin_P0_30) },
    { MP_ROM_QSTR(MP_QSTR_P0_31), MP_ROM_PTR(&pin_P0_31) },
    { MP_ROM_QSTR(MP_QSTR_P0_04), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_P0_05), MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_P1_14), MP_ROM_PTR(&pin_P1_14) },
    { MP_ROM_QSTR(MP_QSTR_P1_12), MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_P0_25), MP_ROM_PTR(&pin_P0_25) },
    { MP_ROM_QSTR(MP_QSTR_P0_11), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_P1_08), MP_ROM_PTR(&pin_P1_08) },
    { MP_ROM_QSTR(MP_QSTR_P0_27), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_P0_08), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_P0_06), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_P0_26), MP_ROM_PTR(&pin_P0_26) },

    { MP_ROM_QSTR(MP_QSTR_P0_10), MP_ROM_PTR(&pin_P0_10) },
    { MP_ROM_QSTR(MP_QSTR_P0_09), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_P1_06), MP_ROM_PTR(&pin_P1_06) },
    { MP_ROM_QSTR(MP_QSTR_P1_04), MP_ROM_PTR(&pin_P1_04) },
    { MP_ROM_QSTR(MP_QSTR_P1_02), MP_ROM_PTR(&pin_P1_02) },
    { MP_ROM_QSTR(MP_QSTR_P1_01), MP_ROM_PTR(&pin_P1_01) },
    { MP_ROM_QSTR(MP_QSTR_P1_03), MP_ROM_PTR(&pin_P1_03) },
    { MP_ROM_QSTR(MP_QSTR_P1_00), MP_ROM_PTR(&pin_P1_00) },
    { MP_ROM_QSTR(MP_QSTR_P0_22), MP_ROM_PTR(&pin_P0_22) },
    { MP_ROM_QSTR(MP_QSTR_P1_07), MP_ROM_PTR(&pin_P1_07) },
    { MP_ROM_QSTR(MP_QSTR_P1_05), MP_ROM_PTR(&pin_P1_05) },
    { MP_ROM_QSTR(MP_QSTR_P0_24), MP_ROM_PTR(&pin_P0_24) },
    { MP_ROM_QSTR(MP_QSTR_P0_20), MP_ROM_PTR(&pin_P0_20) },
    { MP_ROM_QSTR(MP_QSTR_P0_17), MP_ROM_PTR(&pin_P0_17) },
    { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
    { MP_ROM_QSTR(MP_QSTR_P0_14), MP_ROM_PTR(&pin_P0_14) },
    { MP_ROM_QSTR(MP_QSTR_P0_13), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_P0_16), MP_ROM_PTR(&pin_P0_16) },

    { MP_ROM_QSTR(MP_QSTR_P0_07), MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_P1_09), MP_ROM_PTR(&pin_P1_09) },
    { MP_ROM_QSTR(MP_QSTR_P0_12), MP_ROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_P0_23), MP_ROM_PTR(&pin_P0_23) },
    { MP_ROM_QSTR(MP_QSTR_P0_21), MP_ROM_PTR(&pin_P0_21) },
    { MP_ROM_QSTR(MP_QSTR_P0_19), MP_ROM_PTR(&pin_P0_19) },

    // RESET { MP_ROM_QSTR(MP_QSTR_P0_18), MP_ROM_PTR(&pin_P0_18) }

};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
