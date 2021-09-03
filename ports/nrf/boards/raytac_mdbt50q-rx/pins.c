#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_P0_15) },

    { MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_INVERTED_LED), MP_ROM_PTR(&pin_P1_13) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
