#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    // D (Digital only) pins (D0,D1)
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_GPIO6) },        // GPIO6 - D0
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO7) },        // GPIO7 - D1

    // A (ADC) pins (A0,A1)
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },       // GPIO26 - A0 | ADC0
    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_PTR(&pin_GPIO26) },     //              ADC0 alias
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27) },       // GPIO27 - A1 | ADC1
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_PTR(&pin_GPIO27) },     //              ADC1 alias

    // G (General/BUS) pins (G0-G7, G8 NC, G9-G10, G11 NC)
    { MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_GPIO16) },       // GPIO16 - G0  | BUS0
    { MP_ROM_QSTR(MP_QSTR_BUS0), MP_ROM_PTR(&pin_GPIO16) },     //              BUS0 alias
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_GPIO17) },       // GPIO17 - G1  | BUS1
    { MP_ROM_QSTR(MP_QSTR_BUS1), MP_ROM_PTR(&pin_GPIO17) },     //              BUS1 alias
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_GPIO18) },       // GPIO18 - G2  | BUS2
    { MP_ROM_QSTR(MP_QSTR_BUS2), MP_ROM_PTR(&pin_GPIO18) },     //              BUS2 alias
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_GPIO19) },       // GPIO19 - G3  | BUS3
    { MP_ROM_QSTR(MP_QSTR_BUS3), MP_ROM_PTR(&pin_GPIO19) },     //              BUS3 alias
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO20) },       // GPIO20 - G4  | BUS4 | SPI_CIPO
    { MP_ROM_QSTR(MP_QSTR_BUS4), MP_ROM_PTR(&pin_GPIO20) },     //              BUS4 alias
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_GPIO21) },       // GPIO21 - G5  | BUS5 | SPI_CS
    { MP_ROM_QSTR(MP_QSTR_BUS5), MP_ROM_PTR(&pin_GPIO21) },     //              BUS5 alias
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_GPIO22) },       // GPIO22 - G6  | BUS6 | SPI_SCK
    { MP_ROM_QSTR(MP_QSTR_BUS6), MP_ROM_PTR(&pin_GPIO22) },     //              BUS6 alias
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_GPIO23) },       // GPIO23 - G7  | BUS7 | SPI_COPI
    { MP_ROM_QSTR(MP_QSTR_BUS7), MP_ROM_PTR(&pin_GPIO23) },     //              BUS7 alias
                                                                // NC   - G8
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_GPIO28) },       // GPIO28- G9  | BUS9  | ADC_D- | CAM_HSYNC
    { MP_ROM_QSTR(MP_QSTR_BUS9), MP_ROM_PTR(&pin_GPIO28) },     //              BUS9 alias
    { MP_ROM_QSTR(MP_QSTR_ADC_DM), MP_ROM_PTR(&pin_GPIO28) },   //              ADC_DM alias
    { MP_ROM_QSTR(MP_QSTR_CAM_HSYNC), MP_ROM_PTR(&pin_GPIO28) }, //              CAM_HSYNC alias
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_GPIO25) },      // GPIO25 - G10 | BUS10 | ADC_D+ | CAM_VSYNC
    { MP_ROM_QSTR(MP_QSTR_BUS10), MP_ROM_PTR(&pin_GPIO25) },    //              BUS10 alias
    { MP_ROM_QSTR(MP_QSTR_ADC_DP), MP_ROM_PTR(&pin_GPIO25) },   //              ADC_DP alias
    { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC), MP_ROM_PTR(&pin_GPIO25) }, //              CAM_VSYNC alias
                                                                 // NC   - G11

    // PWM pins (PWM0,PWM1)
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_GPIO13) },     // GPIO13 - PWM0
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_GPIO24) },     // GPIO24 - PWM1 | AUD_MCLK

    // AUD (audio)
    { MP_ROM_QSTR(MP_QSTR_AUD_MCLK), MP_ROM_PTR(&pin_GPIO24) }, // GPIO24 - AUD_MCLK | PWM1
    { MP_ROM_QSTR(MP_QSTR_AUD_OUT), MP_ROM_PTR(&pin_GPIO10) },  // GPIO10 - AUD_OUT | SDIO_DAT2
    { MP_ROM_QSTR(MP_QSTR_AUD_IN), MP_ROM_PTR(&pin_GPIO11) },   // GPIO11 - AUD_IN | SDIO_DAT1
    { MP_ROM_QSTR(MP_QSTR_AUD_LRCLK), MP_ROM_PTR(&pin_GPIO2) }, // GPIO2 - AUD_LRCLK | CTS1
    { MP_ROM_QSTR(MP_QSTR_AUD_BCLK), MP_ROM_PTR(&pin_GPIO3) },  // GPIO3 - AUD_BCLK | UART_RTS1

    // Battery Voltage Monitor
    { MP_ROM_QSTR(MP_QSTR_BATT_VIN3), MP_ROM_PTR(&pin_GPIO29) }, // GPIO29 - BATT_VIN/3 (ADC03)


    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },       // GPIO4 - SDA
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO5) },       // GPIO5 - SCL

    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_GPIO8) },   // GPIO9 - I2C_INT | TX2

    // SPI
    { MP_ROM_QSTR(MP_QSTR_CIPO), MP_ROM_PTR(&pin_GPIO20) },     // GPIO20 - CIPO | SPI_CIPO | G4
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO20) },     //              MISO alias
    { MP_ROM_QSTR(MP_QSTR_COPI), MP_ROM_PTR(&pin_GPIO23) },     // GPIO23 - COPI | SPI_COPI | G7
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO23) },     //              MOSI alias
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO22) },      // GPIO22 - SCK  | SPI_SCK | G6
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_GPIO21) },       // GPIO21 - /CS  | SPI_/CS | G5

    // SDI/SPI1
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO14) }, // GPIO14 - SDIO SCK | SDIO_CLK
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK1), MP_ROM_PTR(&pin_GPIO14) }, //              SPI_SCK1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO15) }, // GPIO15 - SDIO CMD | SDIO_CMD
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI1), MP_ROM_PTR(&pin_GPIO15) },//             SPI_COPI1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_GPIO12) },// GPIO12 - SDIO DATA0 | SDIO_DATA0
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO1), MP_ROM_PTR(&pin_GPIO12) }, //             SPI_CIPO1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_GPIO11) },// GPIO11 - SDIO DATA1 | SDIO_DATA1 | AUD_IN
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_GPIO10) },// GPIO10 - SDIO DATA2 | SDIO_DATA2 | AUD_OUT
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA3), MP_ROM_PTR(&pin_GPIO9) },// GPIO9 - SDIO DATA3 | SDIO_DATA3 | SPI_CS1
    { MP_ROM_QSTR(MP_QSTR_SPI_CS1), MP_ROM_PTR(&pin_GPIO9) },   //              SPI_CS1 alias

    // Status LED
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_GPIO25) },     // GPIO25 - LED_BUILTIN | STAT | Blue LED | G10

    // UART
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO1) },        // GPIO1 - UART RX | UART_RX1 | RX1
    { MP_ROM_QSTR(MP_QSTR_RX1), MP_ROM_PTR(&pin_GPIO1) },       //              RX1 alias
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO0) },        // GPIO0 - UART TX | UART_TX1  | TX1
    { MP_ROM_QSTR(MP_QSTR_TX1), MP_ROM_PTR(&pin_GPIO0) },       //              TX1 alias
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_PTR(&pin_GPIO2) },       // GPIO2 - UART CTS | CTS1 (TRACEDATA3)
    { MP_ROM_QSTR(MP_QSTR_CTS1), MP_ROM_PTR(&pin_GPIO2) },      //              CTS1 alias
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_PTR(&pin_GPIO3) },       // GPIO3 - UART RTS | RTS1
    { MP_ROM_QSTR(MP_QSTR_RTS1), MP_ROM_PTR(&pin_GPIO3) },      //              RTS1 alias

    { MP_ROM_QSTR(MP_QSTR_RX2), MP_ROM_PTR(&pin_GPIO9) },       // GPIO9 - UART RX | UART_RX2 | RX2 | SDIO_DAT3 | SPI_CS1
    { MP_ROM_QSTR(MP_QSTR_TX2), MP_ROM_PTR(&pin_GPIO8) },       // GPIO8 - UART TX | UART_TX2 | TX2 | I2C_INT

    // Board objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
