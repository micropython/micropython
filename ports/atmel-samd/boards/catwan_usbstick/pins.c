#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA30) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA31) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_D0), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_D1), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_D2), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_D5), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_RST), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_RFM9X_CS), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA19) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
