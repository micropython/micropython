#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {

    /* default ports */
    { MP_ROM_QSTR(MP_QSTR_UART),    MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI),     MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C),     MP_ROM_PTR(&board_i2c_obj) },

    /* TG-Watch02 specific pins */
    { MP_ROM_QSTR(MP_QSTR_VBUS_PRESENT),    MP_ROM_PTR(&pin_P1_04) },
    { MP_ROM_QSTR(MP_QSTR_HAPTIC_ENABLE),   MP_ROM_PTR(&pin_P1_06) },
    { MP_ROM_QSTR(MP_QSTR_HAPTIC_INT),      MP_ROM_PTR(&pin_P1_07) },
    { MP_ROM_QSTR(MP_QSTR_CTP_INT),         MP_ROM_PTR(&pin_P1_05) },
    { MP_ROM_QSTR(MP_QSTR_CTP_RST),         MP_ROM_PTR(&pin_P1_03) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RST),         MP_ROM_PTR(&pin_P1_01) },

    { MP_ROM_QSTR(MP_QSTR_TFT_DC),  MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_D21),     MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS),  MP_ROM_PTR(&pin_P1_14) },

    { MP_ROM_QSTR(MP_QSTR_ACCEL_INT1), MP_ROM_PTR(&pin_P1_11) },
    { MP_ROM_QSTR(MP_QSTR_ACCEL_INT2), MP_ROM_PTR(&pin_P1_10) },

    { MP_ROM_QSTR(MP_QSTR_BATTERY_DIV), MP_ROM_PTR(&pin_P0_29) },

    { MP_ROM_QSTR(MP_QSTR_RTC_INT),     MP_ROM_PTR(&pin_P0_27) },
    { MP_ROM_QSTR(MP_QSTR_RTC_RST),     MP_ROM_PTR(&pin_P0_26) },
    { MP_ROM_QSTR(MP_QSTR_CHRG_STAT),   MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_BACKLIGHT),   MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_SMC_RST),     MP_ROM_PTR(&pin_P0_08) },

    /* nrf52840 compatible pins */
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_P0_30) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_P0_03) },

    { MP_ROM_QSTR(MP_QSTR_AREF), MP_ROM_PTR(&pin_P0_31) },

    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_BATTERY), MP_ROM_PTR(&pin_P0_29) },

    { MP_ROM_QSTR(MP_QSTR_SWITCH), MP_ROM_PTR(&pin_P1_02) },

    { MP_ROM_QSTR(MP_QSTR_NFC1), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_NFC2), MP_ROM_PTR(&pin_P0_10) },

    { MP_ROM_QSTR(MP_QSTR_D2),  MP_ROM_PTR(&pin_P0_10) },
    { MP_ROM_QSTR(MP_QSTR_D5),  MP_ROM_PTR(&pin_P1_08) },
    { MP_ROM_QSTR(MP_QSTR_D6),  MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_D9),  MP_ROM_PTR(&pin_P0_26) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_P0_27) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_P1_09) },

    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_P0_16) },

    { MP_ROM_QSTR(MP_QSTR_SCK),  MP_ROM_PTR(&pin_P0_14) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_15) },

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P0_25) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P0_24) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_12) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
