#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_REMOTEIN),   MP_ROM_PTR(&pin_PA28) },

    { MP_ROM_QSTR(MP_QSTR_APA102_MOSI), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_DATA), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_APA102_SCK),  MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_CLOCK),  MP_ROM_PTR(&pin_PA01) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
