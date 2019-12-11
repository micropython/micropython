#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_PTR(&pin_PB05) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
