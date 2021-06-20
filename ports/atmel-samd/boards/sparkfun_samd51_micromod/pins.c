#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    // D (digital only) pins (D0,D1)
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PB05) },

    // A (ADC) pins (A0-A4)
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PB03) },

    // DAC
    { MP_ROM_QSTR(MP_QSTR_DAC), MP_ROM_PTR(&pin_PA02) },

    // G (General/BUS) pins (G0-G9)
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_G8), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_D32), MP_ROM_PTR(&pin_PB31) },
    { MP_ROM_QSTR(MP_QSTR_D33), MP_ROM_PTR(&pin_PB30) },
    { MP_ROM_QSTR(MP_QSTR_D38), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_D39), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_G11), MP_ROM_PTR(&pin_PB15) },

    // PWM pins (PWM0, PWM1)
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_PB02) },

    // AUD (audio)
    { MP_ROM_QSTR(MP_QSTR_AUD_MCLK), MP_ROM_PTR(&pin_PB17) },
    { MP_ROM_QSTR(MP_QSTR_AUD_OUT), MP_ROM_PTR(&pin_PA21) },
    { MP_ROM_QSTR(MP_QSTR_AUD_IN), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_AUD_LRCLK), MP_ROM_PTR(&pin_PA20) },
    { MP_ROM_QSTR(MP_QSTR_AUD_BCLK), MP_ROM_PTR(&pin_PB16) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_SDA1), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_SCL1), MP_ROM_PTR(&pin_PA16) },

    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_PA18) },

    // I2C2
    { MP_ROM_QSTR(MP_QSTR_SDA2), MP_ROM_PTR(&pin_PA13) },
    { MP_ROM_QSTR(MP_QSTR_SCL2), MP_ROM_PTR(&pin_PA12) },

    // SPI
    { MP_ROM_QSTR(MP_QSTR_CIPO), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_COPI), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PA07) },

    // Status LED
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PA23) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PB30) },
    { MP_ROM_QSTR(MP_QSTR_RX1), MP_ROM_PTR(&pin_PB30) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PB31) },
    { MP_ROM_QSTR(MP_QSTR_TX1), MP_ROM_PTR(&pin_PB31) },

    // UART2
    { MP_ROM_QSTR(MP_QSTR_RX2), MP_ROM_PTR(&pin_PA13) },
    { MP_ROM_QSTR(MP_QSTR_TX2), MP_ROM_PTR(&pin_PA12) },


    // Board objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
