#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_PTR(&pin_PB05) },

    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_SCK), MP_ROM_PTR(&pin_PB13) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_MISO), MP_ROM_PTR(&pin_PB14) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_MOSI), MP_ROM_PTR(&pin_PB15) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_CS), MP_ROM_PTR(&pin_PB12) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_DC), MP_ROM_PTR(&pin_PA08) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_RST), MP_ROM_PTR(&pin_PB10) },
    // { MP_ROM_QSTR(MP_QSTR_LED_DISP_BL), MP_ROM_PTR(&pin_PB03) }

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
