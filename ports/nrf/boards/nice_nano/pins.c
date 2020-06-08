#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_P0_02), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_P0_04), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_P0_06), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_P0_08), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_P0_09), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_P0_10), MP_ROM_PTR(&pin_P0_10) },
    { MP_ROM_QSTR(MP_QSTR_P0_11), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_P0_12), MP_ROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_P0_13), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
    { MP_ROM_QSTR(MP_QSTR_P0_17), MP_ROM_PTR(&pin_P0_17) },
    { MP_ROM_QSTR(MP_QSTR_P0_20), MP_ROM_PTR(&pin_P0_20) },
    { MP_ROM_QSTR(MP_QSTR_P0_22), MP_ROM_PTR(&pin_P0_22) },
    { MP_ROM_QSTR(MP_QSTR_P0_24), MP_ROM_PTR(&pin_P0_24) },
    { MP_ROM_QSTR(MP_QSTR_P0_26), MP_ROM_PTR(&pin_P0_26) },
    { MP_ROM_QSTR(MP_QSTR_P0_29), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_P0_31), MP_ROM_PTR(&pin_P0_31) },
    { MP_ROM_QSTR(MP_QSTR_P1_00), MP_ROM_PTR(&pin_P1_00) },
    { MP_ROM_QSTR(MP_QSTR_P1_01), MP_ROM_PTR(&pin_P1_01) },
    { MP_ROM_QSTR(MP_QSTR_P1_02), MP_ROM_PTR(&pin_P1_02) },
    { MP_ROM_QSTR(MP_QSTR_P1_04), MP_ROM_PTR(&pin_P1_04) },
    { MP_ROM_QSTR(MP_QSTR_P1_06), MP_ROM_PTR(&pin_P1_06) },
    { MP_ROM_QSTR(MP_QSTR_P1_07), MP_ROM_PTR(&pin_P1_07) },
    { MP_ROM_QSTR(MP_QSTR_P1_11), MP_ROM_PTR(&pin_P1_11) },
    { MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_P1_15), MP_ROM_PTR(&pin_P1_15) },

    { MP_ROM_QSTR(MP_QSTR_AIN0), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_AIN2), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_AIN5), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_AIN7), MP_ROM_PTR(&pin_P0_31) },

    { MP_ROM_QSTR(MP_QSTR_NFC1), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_NFC2), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_BAT_VOLT), MP_ROM_PTR(&pin_P0_04) }, // Read battery voltage

    { MP_ROM_QSTR(MP_QSTR_VCC_OFF), MP_ROM_PTR(&pin_P0_13) }, // Turn off external VCC by MOSFET

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_P0_15) }, // Controls blue LED, high is on

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P0_06) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_20) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_17) },

    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_10) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P1_11) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
