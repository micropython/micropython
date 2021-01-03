#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_P0_02) },      // TP7
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_P0_03) },      // TP6
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_P0_04) },      // LED1
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_P0_05) },      // U2-BMX055-INT1
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_P0_06) },      // U2-BMX055-DRDYM
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_P0_07) },      // LED2
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_P0_08) },      // U4-HDC2010-DRDY/INT
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_P0_09) },      // TP1
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_P0_10) },     // U3-LPS22HB-INT_DRDY
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_P0_12) },     // S2
    { MP_ROM_QSTR(MP_QSTR_D23), MP_ROM_PTR(&pin_P0_23) },     // BZ1
    { MP_ROM_QSTR(MP_QSTR_D28), MP_ROM_PTR(&pin_P0_28) },     // U2-BMX055-INT4
    { MP_ROM_QSTR(MP_QSTR_D29), MP_ROM_PTR(&pin_P0_29) },     // U2-BMX055-INT3
    { MP_ROM_QSTR(MP_QSTR_D30), MP_ROM_PTR(&pin_P0_30) },     // U2-BMX055-INT5
    { MP_ROM_QSTR(MP_QSTR_D31), MP_ROM_PTR(&pin_P0_31) },     // S1

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_02) },      // A0/TP7
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_03) },      // A1/TP6

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_24) },     // 24 - SDA
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_26) },     // 26 - SCL

    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_13) },    // 13 - MISO (n.c)
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_14) },    // 14 - MOSI (n.c)
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_15) },     // 15 - SCK (n.c)

    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_P0_07)  },   // 4 - LED1
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_P0_04)  },   // 7 - LED2

    { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_P0_31) }, // 31 - S1
    { MP_ROM_QSTR(MP_QSTR_BUTTON2), MP_ROM_PTR(&pin_P0_12) }, // 12 - S2

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P0_02) },      // 2 - UART RX
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P0_03) },      // 3 - UART TX

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_QWIIC), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
