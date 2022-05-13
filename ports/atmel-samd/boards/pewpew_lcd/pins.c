#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR__SCK), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR__MOSI), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR__CS), MP_ROM_PTR(&pin_PA19) },
    { MP_ROM_QSTR(MP_QSTR__RST), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR__BL), MP_ROM_PTR(&pin_PA17) },

    { MP_ROM_QSTR(MP_QSTR__UP), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR__DOWN), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR__LEFT), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR__RIGHT), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR__O), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR__X), MP_ROM_PTR(&pin_PA07) },

    { MP_ROM_QSTR(MP_QSTR_P1), MP_ROM_PTR(&pin_PA30) },
    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_PA31) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_PA14) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
