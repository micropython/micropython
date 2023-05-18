#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(bm8563_i2c, i2c, 1) // RTC
CIRCUITPY_BOARD_BUS_SINGLETON(sscb_i2c, i2c, 2) // Camera sensor

STATIC const mp_rom_obj_tuple_t camera_data_tuple = {
    // The order matters.
    // They must be ordered from low to high (Y2, Y3 .. Y9).

    // Do not include any of the control pins in here.
    {&mp_type_tuple},
    8,
    {
        MP_ROM_PTR(&pin_GPIO32), // Y2
        MP_ROM_PTR(&pin_GPIO35), // Y3
        MP_ROM_PTR(&pin_GPIO34), // Y4
        MP_ROM_PTR(&pin_GPIO5),  // Y5
        MP_ROM_PTR(&pin_GPIO39), // Y6
        MP_ROM_PTR(&pin_GPIO18), // Y7
        MP_ROM_PTR(&pin_GPIO36), // Y8
        MP_ROM_PTR(&pin_GPIO19)  // Y9
    }
};

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO2) },

    // "Ext. Port", the label says [G, V, SDA/G4, SCL/G13]
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_G13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    /* Battery
     *
     * No alternative names as they are not broken out
     * and cannot be used for anything else.
     */
    { MP_ROM_QSTR(MP_QSTR_BAT_HOLD), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_BAT_ADC), MP_ROM_PTR(&pin_GPIO38) },

    /* BM8563 I2C bus
     *
     * No alternative names as they are not broken out
     * and cannot be used for anything else.
     */
    { MP_ROM_QSTR(MP_QSTR_BM8563_SCL), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_BM8563_SDA), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_BM8563_I2C), MP_ROM_PTR(&board_bm8563_i2c_obj) },

    // Camera control
    { MP_ROM_QSTR(MP_QSTR_VSYNC), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_HREF), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_PCLK), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_XCLK), MP_ROM_PTR(&pin_GPIO27) }, // xclk_freq_hz = 20000000
    { MP_ROM_QSTR(MP_QSTR_RESET), MP_ROM_PTR(&pin_GPIO15) },

    // Camera sensor I2C bus
    { MP_ROM_QSTR(MP_QSTR_SSCB_SDA), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_SSCB_SCL), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_SSCB_I2C), MP_ROM_PTR(&board_sscb_i2c_obj) },

    /*
     * Camera data
     *
     * We don't really need to individually name each one,
     * but they look cool in the tuple listing if we do.
     *
     * These are also not broken out.
     */
    { MP_ROM_QSTR(MP_QSTR_D), MP_ROM_PTR(&camera_data_tuple) },

    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO32) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
