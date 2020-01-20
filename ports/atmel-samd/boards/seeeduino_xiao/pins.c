#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {

    // Analog pins
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A9), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PA06) },

    // Digital pins
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA17) },

    // UART pins
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PB09) },

    // SPI pins
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA05) },

    // I2C pins
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA08) },

    // LED pins
    { MP_ROM_QSTR(MP_QSTR_LED),   MP_ROM_PTR(&pin_PA17) }, // status
    { MP_ROM_QSTR(MP_QSTR_BLUE_LED),   MP_ROM_PTR(&pin_PA17) },

    // Comm objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
