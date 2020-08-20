#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_LEFT), MP_ROM_PTR(&pin_PB23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_RIGHT), MP_ROM_PTR(&pin_PB22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_PTR(&pin_PA27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_O), MP_ROM_PTR(&pin_PA10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_X), MP_ROM_PTR(&pin_PA09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_Z), MP_ROM_PTR(&pin_PA22) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_P1), MP_ROM_PTR(&pin_PA30) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_PA31) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_PA01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_PA05) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_RST), MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_DC),  MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_SCK), MP_ROM_PTR(&pin_PA13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_PA11) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_PA02) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA01) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA00) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA00) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
