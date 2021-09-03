#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO21) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),MP_ROM_PTR(&pin_GPIO33) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),MP_ROM_PTR(&pin_GPIO34) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO45) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
