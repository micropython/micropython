#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB23) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB22) },
    { MP_ROM_QSTR(MP_QSTR_DAC_CS), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_PA20) },
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_PA21) },
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
