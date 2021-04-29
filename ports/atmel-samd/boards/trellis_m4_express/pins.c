#include "shared-bindings/board/__init__.h"

// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1),  MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2),  MP_ROM_PTR(&pin_PA04) }, // INT pin
    { MP_OBJ_NEW_QSTR(MP_QSTR_MICOUT),  MP_ROM_PTR(&pin_PA06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MICIN),  MP_ROM_PTR(&pin_PA07) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB09) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_ACCELEROMETER_SDA), MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ACCELEROMETER_SCL), MP_ROM_PTR(&pin_PA13) },

    // Key Grid columns
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL0),  MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL1),  MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL2),  MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL3),  MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL4),  MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL5),  MP_ROM_PTR(&pin_PA21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL6),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL7),  MP_ROM_PTR(&pin_PA23) },
    // Key Grid rows
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW0),  MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW1),  MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW2),  MP_ROM_PTR(&pin_PB22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW3),  MP_ROM_PTR(&pin_PB23) },

    // IRQ/GPIO pin
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT), MP_ROM_PTR(&pin_PA04) },

    // NeoPixels
    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA27) },

    { MP_ROM_QSTR(MP_QSTR_APA102_MOSI), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_DATA), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_APA102_SCK), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_CLOCK), MP_ROM_PTR(&pin_PB02) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
