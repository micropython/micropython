#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {


    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT), MP_ROM_PTR(&pin_P1_10) },
    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_SDA),       MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_SCL),       MP_ROM_PTR(&pin_P1_11) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);