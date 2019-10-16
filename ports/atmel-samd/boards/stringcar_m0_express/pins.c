#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PIEZO), MP_ROM_PTR(&pin_PA08) },

    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_PA02) },

    { MP_ROM_QSTR(MP_QSTR_SENSOR_IN), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_OUT_1), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_OUT_2), MP_ROM_PTR(&pin_PA07) },

    { MP_ROM_QSTR(MP_QSTR_LED_STATUS), MP_ROM_PTR(&pin_PA10) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA04) },

    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_DI), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_CI), MP_ROM_PTR(&pin_PA01) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
