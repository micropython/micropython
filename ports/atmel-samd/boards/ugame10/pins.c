#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_X), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_O), MP_ROM_PTR(&pin_PA01) },

    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_MUTE), MP_ROM_PTR(&pin_PA23) },

    { MP_ROM_QSTR(MP_QSTR_BATTERY), MP_ROM_PTR(&pin_PA10) },

    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_PTR(&pin_PA05) },

    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_DC), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PA08) },

    { MP_ROM_QSTR(MP_QSTR_A), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_B), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_C), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_D), MP_ROM_PTR(&pin_PA28) },

    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
