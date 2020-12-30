#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_HONK_OUT), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_GATE_OUT), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_GATE_IN), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_PITCH_IN), MP_ROM_PTR(&pin_PB08) },
    /* Board revisions starting from v5 have PB10 tied to ground. */
    { MP_ROM_QSTR(MP_QSTR_V5), MP_ROM_PTR(&pin_PB10) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
