#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA02) }, // IRQ
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PA06) }, // IN_A
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PA07) }, // IN_B
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA05) }, // LED
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA04) }, // LED
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PA14) }, // LED
    { MP_ROM_QSTR(MP_QSTR_D27), MP_ROM_PTR(&pin_PA27) }, // Switch
    { MP_ROM_QSTR(MP_QSTR_D28), MP_ROM_PTR(&pin_PA28) }, // Switch
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA19) },
    { MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_PA18) }, // CS
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
