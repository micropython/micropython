#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_8), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_9), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_10), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_15), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_16), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_23), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_24), MP_ROM_PTR(&pin_GPIO24) },
    { MP_ROM_QSTR(MP_QSTR_25), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_26), MP_ROM_PTR(&pin_GPIO26) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_27), MP_ROM_PTR(&pin_GPIO27) },

    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_28), MP_ROM_PTR(&pin_GPIO28) },

    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO28) },

    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO29) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
