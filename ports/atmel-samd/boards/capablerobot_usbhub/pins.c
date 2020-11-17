#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_ANMB),   MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ANVLIM), MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AN5V),   MP_ROM_PTR(&pin_PA05) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MBPWM),  MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MBINT),  MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MBCS),   MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MBRST),  MP_ROM_PTR(&pin_PB09) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED1),   MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED2),   MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED3),   MP_ROM_PTR(&pin_PA21) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_RX),     MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX),     MP_ROM_PTR(&pin_PA17) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_USBHOSTEN),  MP_ROM_PTR(&pin_PA07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USBRESET),   MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USBBCEN),    MP_ROM_PTR(&pin_PB22) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),    MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),    MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA2),   MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL2),   MP_ROM_PTR(&pin_PA13) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK),    MP_ROM_PTR(&pin_PA01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI),   MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO),   MP_ROM_PTR(&pin_PB23) },

    { MP_ROM_QSTR(MP_QSTR_I2C),        MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI),        MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART),       MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
