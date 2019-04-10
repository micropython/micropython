#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_P0_00), MP_ROM_PTR(&pin_P0_00) },
    { MP_ROM_QSTR(MP_QSTR_P0_01), MP_ROM_PTR(&pin_P0_01) },
    { MP_ROM_QSTR(MP_QSTR_P0_02), MP_ROM_PTR(&pin_P0_02) },

    { MP_ROM_QSTR(MP_QSTR_P0_03), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_03) },

    { MP_ROM_QSTR(MP_QSTR_P0_04), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_04) },

    { MP_ROM_QSTR(MP_QSTR_P0_05), MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_P0_06), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_P0_07), MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_P0_08), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_P0_09), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_P0_10), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_P0_11), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_P0_11) },

    { MP_ROM_QSTR(MP_QSTR_P0_12), MP_ROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON2), MP_ROM_PTR(&pin_P0_12) },

    { MP_ROM_QSTR(MP_QSTR_P0_13), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_L), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_P0_13) },

    { MP_ROM_QSTR(MP_QSTR_P0_14), MP_ROM_PTR(&pin_P0_14) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_P0_14) },

    { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_P0_15) },

    { MP_ROM_QSTR(MP_QSTR_P0_16), MP_ROM_PTR(&pin_P0_16) },
    { MP_ROM_QSTR(MP_QSTR_LED4), MP_ROM_PTR(&pin_P0_16) },

    { MP_ROM_QSTR(MP_QSTR_P0_17), MP_ROM_PTR(&pin_P0_17) },

    // RESET { MP_ROM_QSTR(MP_QSTR_P0_18), MP_ROM_PTR(&pin_P0_18) },

    { MP_ROM_QSTR(MP_QSTR_P0_19), MP_ROM_PTR(&pin_P0_19) },
    { MP_ROM_QSTR(MP_QSTR_P0_20), MP_ROM_PTR(&pin_P0_20) },
    { MP_ROM_QSTR(MP_QSTR_P0_21), MP_ROM_PTR(&pin_P0_21) },
    { MP_ROM_QSTR(MP_QSTR_P0_22), MP_ROM_PTR(&pin_P0_22) },
    { MP_ROM_QSTR(MP_QSTR_P0_23), MP_ROM_PTR(&pin_P0_23) },

    { MP_ROM_QSTR(MP_QSTR_P0_24), MP_ROM_PTR(&pin_P0_24) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON3), MP_ROM_PTR(&pin_P0_24) },

    { MP_ROM_QSTR(MP_QSTR_P0_25), MP_ROM_PTR(&pin_P0_25) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON4), MP_ROM_PTR(&pin_P0_25) },

    { MP_ROM_QSTR(MP_QSTR_P0_26), MP_ROM_PTR(&pin_P0_26) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_26) },

    { MP_ROM_QSTR(MP_QSTR_P0_27), MP_ROM_PTR(&pin_P0_27) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_27) },

    { MP_ROM_QSTR(MP_QSTR_P0_28), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_P0_28) },

    { MP_ROM_QSTR(MP_QSTR_P0_29), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_P0_29) },

    { MP_ROM_QSTR(MP_QSTR_P0_30), MP_ROM_PTR(&pin_P0_30) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_P0_30) },

    { MP_ROM_QSTR(MP_QSTR_P0_31), MP_ROM_PTR(&pin_P0_31) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_P0_31) },

    { MP_ROM_QSTR(MP_QSTR_P1_00), MP_ROM_PTR(&pin_P1_00) },

    { MP_ROM_QSTR(MP_QSTR_P1_01), MP_ROM_PTR(&pin_P1_01) },
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_P1_01) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P1_01) },

    { MP_ROM_QSTR(MP_QSTR_P1_02), MP_ROM_PTR(&pin_P1_02) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_P1_02) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P1_02) },

    { MP_ROM_QSTR(MP_QSTR_P1_03), MP_ROM_PTR(&pin_P1_03) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_P1_03) },

    { MP_ROM_QSTR(MP_QSTR_P1_04), MP_ROM_PTR(&pin_P1_04) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_P1_04) },

    { MP_ROM_QSTR(MP_QSTR_P1_05), MP_ROM_PTR(&pin_P1_05) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_P1_05) },

    { MP_ROM_QSTR(MP_QSTR_P1_06), MP_ROM_PTR(&pin_P1_06) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_P1_06) },

    { MP_ROM_QSTR(MP_QSTR_P1_07), MP_ROM_PTR(&pin_P1_07) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_P1_07) },

    { MP_ROM_QSTR(MP_QSTR_P1_08), MP_ROM_PTR(&pin_P1_08) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_P1_08) },

    { MP_ROM_QSTR(MP_QSTR_P1_09), MP_ROM_PTR(&pin_P1_09) },

    { MP_ROM_QSTR(MP_QSTR_P1_10), MP_ROM_PTR(&pin_P1_10) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_P1_10) },

    { MP_ROM_QSTR(MP_QSTR_P1_11), MP_ROM_PTR(&pin_P1_11) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_P1_11) },

    { MP_ROM_QSTR(MP_QSTR_P1_12), MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_P1_12) },

    { MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P1_13) },

    { MP_ROM_QSTR(MP_QSTR_P1_14), MP_ROM_PTR(&pin_P1_14) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_P1_14) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P1_14) },

    // Note that there is no LED on D13.
    { MP_ROM_QSTR(MP_QSTR_P1_15), MP_ROM_PTR(&pin_P1_15) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_P1_15) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P1_15) },

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
