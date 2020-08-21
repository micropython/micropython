#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SPI_CSN), MP_ROM_PTR(&pin_P1_06) },
    { MP_ROM_QSTR(MP_QSTR_SPI_MISO), MP_ROM_PTR(&pin_P1_04) },
    { MP_ROM_QSTR(MP_QSTR_SPI_MOSI), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_NFC1), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_NFC2), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_P0_06) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P1_09) },

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
