#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    // D pins (D0-D1)
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_P0_27) },        // 0.27 - D0
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_P1_08) },        // 1.08 - D1 | CAM_TRIG
    { MP_ROM_QSTR(MP_QSTR_CAM_TRIG), MP_ROM_PTR(&pin_P1_08) },  //              CAM_TRIG alias

    // A pins (A0-A1)
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_04) },        // 0.04 - A0 | ADC0 (AIN2)
    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_PTR(&pin_P0_04) },      //              ADC0 alias
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_05) },        // 0.05 - A1 | ADC1 (AIN3)
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_PTR(&pin_P0_05) },      //              ADC1 alias

    // G pins (G0-G11, G11 NC)
    { MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_P0_29) },        // 0.29 - G0  | GPIO0 | BUS0 (AIN5)
    { MP_ROM_QSTR(MP_QSTR_BUS0), MP_ROM_PTR(&pin_P0_29) },      //              BUS0 alias
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_P0_03) },        // 0.03 - G1  | GPIO1 | BUS1 (AIN1)
    { MP_ROM_QSTR(MP_QSTR_BUS1), MP_ROM_PTR(&pin_P0_03) },      //              BUS1 alias
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_P1_13) },        // 1.13 - G2  | GPIO2 | BUS2
    { MP_ROM_QSTR(MP_QSTR_BUS2), MP_ROM_PTR(&pin_P1_13) },      //              BUS2 alias
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_P1_12) },        // 1.12 - G3  | GPIO3 | BUS3
    { MP_ROM_QSTR(MP_QSTR_BUS3), MP_ROM_PTR(&pin_P1_12) },      //              BUS3 alias
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_P1_11) },        // 1.11 - G4  | GPIO4 | BUS4
    { MP_ROM_QSTR(MP_QSTR_BUS4), MP_ROM_PTR(&pin_P1_11) },      //              BUS4 alias
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_P0_17) },        // 0.17 - G5  | GPIO5 | BUS5
    { MP_ROM_QSTR(MP_QSTR_BUS5), MP_ROM_PTR(&pin_P0_17) },      //              BUS5 alias
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_P1_06) },        // 1.06 - G6  | GPIO6 | BUS6
    { MP_ROM_QSTR(MP_QSTR_BUS6), MP_ROM_PTR(&pin_P1_06) },      //              BUS6 alias
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_P1_04) },        // 1.04 - G7  | GPIO7 | BUS7
    { MP_ROM_QSTR(MP_QSTR_BUS7), MP_ROM_PTR(&pin_P1_04) },      //              BUS7 alias
    { MP_ROM_QSTR(MP_QSTR_G8), MP_ROM_PTR(&pin_P1_14) },        // 1.14 - G8  | GPIO8
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_P0_09) },        // 0.09 - G9  | GPIO9/NFC1  | ADC_D- | CAM_HSYNC (NFC1)
    { MP_ROM_QSTR(MP_QSTR_NFC1), MP_ROM_PTR(&pin_P0_09) },      //              NFC1 alias
    { MP_ROM_QSTR(MP_QSTR_ADC_DM), MP_ROM_PTR(&pin_P0_09) },    //              ADC_DM alias
    { MP_ROM_QSTR(MP_QSTR_CAM_HSYNC), MP_ROM_PTR(&pin_P0_09) }, //              CAM_HSYNC alias
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_P0_10) },       // 0.10 - G10 | GPIO10/NFC2 | ADC_D+ | CAM_VSYNC (NFC2)
    { MP_ROM_QSTR(MP_QSTR_NFC2), MP_ROM_PTR(&pin_P0_10) },      //              NFC2 alias
    { MP_ROM_QSTR(MP_QSTR_ADC_DP), MP_ROM_PTR(&pin_P0_10) },    //              ADC_DP alias
    { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC), MP_ROM_PTR(&pin_P0_10) }, //              CAM_VSYNC alias
                                                                // NC   - G11

    // PWM pins (PWM0-PWM1)
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_P0_06) },      // 0.06 - PWM0
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_P0_16) },      // 0.16 - PWM1

    // PDM
    { MP_ROM_QSTR(MP_QSTR_PDM_CLK), MP_ROM_PTR(&pin_P0_25) },   // 0.25 - PDM_CLK | AUD_BCLK
    { MP_ROM_QSTR(MP_QSTR_PDM_DATA), MP_ROM_PTR(&pin_P0_26) },  // 0.26 - PDM_DATA | AUD_LRCLK

    // Battery Voltage Monitor
    { MP_ROM_QSTR(MP_QSTR_BATT_VIN3), MP_ROM_PTR(&pin_P0_30) }, // 0.30 - BATT_VIN/3 (AIN6)

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_08) },       // 0.08 - SDA
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_11) },       // 0.11 - SCL (TRACEDATA2)

    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_P0_15) },   // 0.15 - I2C_INT

    { MP_ROM_QSTR(MP_QSTR_SDA1), MP_ROM_PTR(&pin_P1_01) },       // 1.01 - SDA1
    { MP_ROM_QSTR(MP_QSTR_SCL1), MP_ROM_PTR(&pin_P0_24) },       // 0.24 - SCL1

    // SPI
    { MP_ROM_QSTR(MP_QSTR_CIPO), MP_ROM_PTR(&pin_P0_02) },      // 0.02 - CIPO | SPI_CIPO
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_02) },      //              MISO alias
    { MP_ROM_QSTR(MP_QSTR_COPI), MP_ROM_PTR(&pin_P0_31) },      // 0.31 - COPI | SPI_COPI (AIN7)
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_31) },      //              MOSI alias
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_28) },       // 0.28 - SCK  | SPI_SCK (AIN4)
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_P0_20) },        // 0.20 - /CS  | SPI_/CS

    // QSPI, used by flash on this board, but is broken out
    // on the MicroMod connector, to to the SDIO pins.
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_P0_19) },  // 0.00 - SDIO SCK | Used as: QSPI flash SCK
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK1), MP_ROM_PTR(&pin_P0_19) },  //              SPI_SCK1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_P0_14) },  // 0.00 - SDIO CMD | Used as: QSPI flash D0 (or SDI)
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI1), MP_ROM_PTR(&pin_P0_14) },  //             SPI_COPI1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_P0_21) },// 0.00 - SDIO DATA0 | Used as: QSPI flash D1 (or SDO)
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO1), MP_ROM_PTR(&pin_P0_21) },  //             SPI_CIPO1 alias
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_P0_22) },// 0.00 - SDIO DATA1 | Unused for flash.
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_P0_23) },// 0.00 - SDIO DATA2 | Used as: QSPI flash D2
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA3), MP_ROM_PTR(&pin_P1_00) },// 0.00 - SDIO DATA3 | Use das: QSPI flash D3 (or /HOLD)
    { MP_ROM_QSTR(MP_QSTR_SPI_CS1), MP_ROM_PTR(&pin_P1_00) },   //              SPI_CS1 alias

    // Reset Pin
    { MP_ROM_QSTR(MP_QSTR_RESET), MP_ROM_PTR(&pin_P1_14) },     // 0.18 - /RESET (NRESET)

    // LED
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_P0_13)  },     // 0.13 - LED_BUILTIN | STAT | Blue LED

    // Button
    { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_P0_07) },   // 0.07 - /BOOT [Active Low] (TRACECLK) - Is button on carriers.
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_P0_07) },      //              BOOT alias

    // UART
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P1_10) },        // 1.10 - UART RX  | RX1
    { MP_ROM_QSTR(MP_QSTR_RX1), MP_ROM_PTR(&pin_P1_10) },       //              RX1 alias
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P1_03) },        // 1.03 - UART TX  | TX1
    { MP_ROM_QSTR(MP_QSTR_TX1), MP_ROM_PTR(&pin_P1_03) },       //              TX1 alias
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_PTR(&pin_P1_09) },       // 1.09 - UART CTS | CTS1 (TRACEDATA3)
    { MP_ROM_QSTR(MP_QSTR_CTS1), MP_ROM_PTR(&pin_P1_09) },      //              CTS1 alias
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_PTR(&pin_P1_02) },       // 1.02 - UART RTS | RTS1
    { MP_ROM_QSTR(MP_QSTR_RTS1), MP_ROM_PTR(&pin_P1_02) },      //              RTS1 alias

    { MP_ROM_QSTR(MP_QSTR_RX2), MP_ROM_PTR(&pin_P1_05) },       // 1.05 - UART RX  | RX2
    { MP_ROM_QSTR(MP_QSTR_TX2), MP_ROM_PTR(&pin_P1_07) },       // 1.07 - UART TX  | TX2

    // Board Objects
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
