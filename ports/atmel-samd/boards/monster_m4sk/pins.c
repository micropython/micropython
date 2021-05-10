#include "shared-bindings/board/__init__.h"

#include "supervisor/board.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HEADPHONE_LEFT),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HEADPHONE_RIGHT),  MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1),  MP_ROM_PTR(&pin_PA05) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_D2),  MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2),  MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NOSE),  MP_ROM_PTR(&pin_PB08) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_D3),  MP_ROM_PTR(&pin_PB09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3),  MP_ROM_PTR(&pin_PB09) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED),  MP_ROM_PTR(&pin_PA27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13),  MP_ROM_PTR(&pin_PA27) },

    // I2C
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),  MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),  MP_ROM_PTR(&pin_PA01) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT),  MP_ROM_PTR(&pin_PA22) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER_ENABLE),  MP_ROM_PTR(&pin_PA14) },

    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_CLOCK), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_DATA), MP_ROM_PTR(&pin_PA17) },

    // Right TFT control pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT_TFT_LITE),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT_TFT_MOSI),  MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT_TFT_SCK),  MP_ROM_PTR(&pin_PA13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT_TFT_RST),  MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT_TFT_CS), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT_TFT_DC), MP_ROM_PTR(&pin_PA07) },

    // Left TFT control pins. Some pins are attached through the SeeSaw chip.
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEFT_TFT_MOSI),  MP_ROM_PTR(&pin_PB02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEFT_TFT_SCK),  MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_LEFT_TFT_CS), MP_ROM_PTR(&pin_PB23) },
    { MP_ROM_QSTR(MP_QSTR_LEFT_TFT_DC), MP_ROM_PTR(&pin_PB22) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},
    { MP_ROM_QSTR(MP_QSTR_RIGHT_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
