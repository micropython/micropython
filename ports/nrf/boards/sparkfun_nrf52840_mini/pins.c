#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_P1_15) },      // D1/TX
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_P0_17) },      // D0/RX
    // D2 on qwiic gap
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_P0_19) },      // D3
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_P0_20) },      // D4
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_P0_21) },      // D5
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_P0_22) },      // D6
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_P0_23) },      // D7
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_P0_09) },      // D8
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_P0_10) },      // D9

    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_P0_02) },     // D10
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_P0_03) },     // D11
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_P0_31) },     // D12
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_P0_30) },     // D13
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_P0_29) },     // D14
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_P0_28) },     // D15
    { MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_P0_05) },     // D16
    { MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_P0_04) },     // D17

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_02) },      // A0
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_03) },      // A1
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_P0_04) },      // A2
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_P0_05) },      // A3
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_P0_28) },      // A4
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_P0_29) },      // A5
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_P0_30) },      // A6
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_P0_31) },      // A7

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_08) },     // 8 - SDA
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_11) },     // 11 - SCL

    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_31) },    // 31 - MISO
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_03) },    // 3 - MOSI
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_30) },     // 30 - SCK

    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_P0_07)  },   // 7 - Blue LED

    { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_P0_13) }, // 13 - Button

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P0_15) },      // 15 - UART RX
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P0_17) },      // 17 - UART TX

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_QWIIC), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
