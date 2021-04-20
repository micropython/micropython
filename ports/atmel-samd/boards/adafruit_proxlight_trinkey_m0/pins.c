#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_INTERRUPT), MP_ROM_PTR(&pin_PA00) },

    { MP_ROM_QSTR(MP_QSTR_TOUCH2), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH1), MP_ROM_PTR(&pin_PA07) },

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA17) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
