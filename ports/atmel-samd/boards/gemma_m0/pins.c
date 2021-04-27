#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_A1),  MP_ROM_PTR(&pin_PA05) }, // pad 1
    { MP_ROM_QSTR(MP_QSTR_D2),  MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_RX),  MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA05) },

    { MP_ROM_QSTR(MP_QSTR_A2),  MP_ROM_PTR(&pin_PA04) }, // pad 0
    { MP_ROM_QSTR(MP_QSTR_D0),  MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_TX),  MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA04) },

    { MP_ROM_QSTR(MP_QSTR_A0),  MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_D1),  MP_ROM_PTR(&pin_PA02) },

    { MP_ROM_QSTR(MP_QSTR_LED),   MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_L),   MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA23) },

    { MP_ROM_QSTR(MP_QSTR_APA102_MOSI), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_DATA), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_APA102_SCK),  MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_DOTSTAR_CLOCK),  MP_ROM_PTR(&pin_PA01) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
