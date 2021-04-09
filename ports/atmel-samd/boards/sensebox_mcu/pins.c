#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {

    // Analog pins
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA02) },

    // Digital pins
    // { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA20) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PA28) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PB09) },

    // UART pins
    { MP_ROM_QSTR(MP_QSTR_UART_PWR), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_TX1), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_RX1), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_TX2), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_RX2), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA22) }, // alias of TX1
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA23) }, // alias of RX1

    // SPI pins
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA19) },

    // I2C pins
    { MP_ROM_QSTR(MP_QSTR_I2C_PWR), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA08) },

    // LED pins
    { MP_ROM_QSTR(MP_QSTR_LED),   MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_RED_LED),   MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_GREEN_LED),   MP_ROM_PTR(&pin_PA28) },

    // XBEE pins
    { MP_ROM_QSTR(MP_QSTR_XB1_PWR),   MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_XB2_PWR),   MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_XB1_CS),   MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_XB2_CS),   MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_XB1_INT),   MP_ROM_PTR(&pin_PA21) },
    { MP_ROM_QSTR(MP_QSTR_XB2_INT),   MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_XB1_RX),   MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_XB1_TX),   MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_XB2_RX),   MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_XB2_TX),   MP_ROM_PTR(&pin_PA10) },


    // Comm objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
