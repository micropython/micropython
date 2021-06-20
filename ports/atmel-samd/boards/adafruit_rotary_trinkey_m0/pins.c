#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_ROTA), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_ROTB), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH), MP_ROM_PTR(&pin_PA06) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
