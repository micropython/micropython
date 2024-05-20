// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2021 Chris Wilson
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS
    // The SparkFun MicroMod spec uses a zero-based peripheral numbering scheme.
    // The 0th peripheral is the default and the "0" is omitted from the
    // peripheral name (e.g. "I2C" instead of "I2C0").
    //
    // For more details, see https://www.sparkfun.com/micromod#tech-specs

    // MicroMod built-in status LED pin
    // Requirement from the "Designing with MicroMod" SparkFun article:
    // "... every Processor Board shall include one status LED connected to a
    // pin that is not connected to the board edge."
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_P0_13) },        // MicroMod LED (P0.13)

    // MicroMod USB bus input voltage (+5V) pin
    // { MP_ROM_QSTR(MP_QSTR_USB_VIN), MP_ROM_PTR() },            // MicroMod USB_VIN (MDBT50Q-P1M has a dedicated HW VBUS pin)

    // MicroMod +3.3V enable pin
    { MP_ROM_QSTR(MP_QSTR_P3V3_EN), MP_ROM_PTR(&pin_P1_15) },     // MicroMod 3.3V_EN (P1.15)

    // MicroMod battery voltage sense pin
    { MP_ROM_QSTR(MP_QSTR_BATT_VIN3), MP_ROM_PTR(&pin_P0_30) },   // MicroMod BATT_VIN/3 (P0.30)

    // MicroMod reset pin
    { MP_ROM_QSTR(MP_QSTR_RESET), MP_ROM_PTR(&pin_P0_18) },       // MicroMod RESET# (P0.18)

    // MicroMod boot pin
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_P0_07) },        // MicroMod BOOT (P0.07)

    // MicroMod USB device pins
    // USB device is always used internally by CircuitPython, so skip creating
    // the pin objects for it.
    // { MP_ROM_QSTR(MP_QSTR_USB_DM), MP_ROM_PTR() },             // MicroMod USB_D- (MDBT50Q-P1M has a dedicated HW D- pin)
    // { MP_ROM_QSTR(MP_QSTR_USB_DP), MP_ROM_PTR() },             // MicroMod USB_D+ (MDBT50Q-P1M has a dedicated HW D+ pin)

    // MicroMod USB host pins
    // { MP_ROM_QSTR(MP_QSTR_USBHOST_DM), MP_ROM_PTR() },         // MicroMod USBHOST_D- (not supported)
    // { MP_ROM_QSTR(MP_QSTR_USBHOST_DP), MP_ROM_PTR() },         // MicroMod USBHOST_D+ (not supported)

    // MicroMod CAN pins
    // { MP_ROM_QSTR(MP_QSTR_CAN_RX), MP_ROM_PTR() },             // MicroMod CAN_RX (not supported)
    // { MP_ROM_QSTR(MP_QSTR_CAN_TX), MP_ROM_PTR() },             // MicroMod CAN_TX (not supported)

    // Note: MicroMod UART (UART0) is not present in the edge connector pinout
    // because the primary debug serial port is exposed as a virtual serial port
    // over USB.

    // MicroMod UART1 pins
    { MP_ROM_QSTR(MP_QSTR_UART_TX1), MP_ROM_PTR(&pin_P1_03) },    // MicroMod UART_TX1 | CircuitPython TX (P1.03)
    { MP_ROM_QSTR(MP_QSTR_UART_RX1), MP_ROM_PTR(&pin_P1_10) },    // MicroMod UART_RX1 | CircuitPython RX (P1.10)
    { MP_ROM_QSTR(MP_QSTR_UART_RTS1), MP_ROM_PTR(&pin_P1_02) },   // MicroMod RTS1 (P1.02)
    { MP_ROM_QSTR(MP_QSTR_UART_CTS1), MP_ROM_PTR(&pin_P1_09) },   // MicroMod CTS1 (P1.09)

    // CircuitPython default UART pins
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_P1_03) },          // CircuitPython TX | MicroMod UART_TX1 (P1.03)
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_P1_10) },          // CircuitPython RX | MicroMod UART_RX1 (P1.10)

    // MicroMod UART2 pins
    { MP_ROM_QSTR(MP_QSTR_UART_TX2), MP_ROM_PTR(&pin_P1_07) },    // MicroMod UART_TX2 (P1.07)
    { MP_ROM_QSTR(MP_QSTR_UART_RX2), MP_ROM_PTR(&pin_P1_05) },    // MicroMod UART_RX2 (P1.05)

    // MicroMod I2C pins
    { MP_ROM_QSTR(MP_QSTR_I2C_SDA), MP_ROM_PTR(&pin_P0_08) },     // MicroMod I2C_SDA (P0.08)
    { MP_ROM_QSTR(MP_QSTR_I2C_SCL), MP_ROM_PTR(&pin_P0_11) },     // MicroMod I2C_SCL (P0.11)

    // CircuitPython default I2C pins
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_08) },         // CircuitPython SDA | MicroMod I2C_SDA (P0.08)
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_11) },         // CircuitPython SCL | MicroMod I2C_SCL (P0.11)

    // MicroMod I2C interrupt pin
    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_P0_15) },     // MicroMod I2C_INT (P0.15)

    // MicroMod I2C1 pins
    { MP_ROM_QSTR(MP_QSTR_I2C_SDA1), MP_ROM_PTR(&pin_P1_01) },    // MicroMod I2C_SDA1 (P1.01)
    { MP_ROM_QSTR(MP_QSTR_I2C_SCL1), MP_ROM_PTR(&pin_P0_24) },    // MicroMod I2C_SCL1 (P0.24)

    // MicroMod SPI pins
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO), MP_ROM_PTR(&pin_P0_02) },    // MicroMod SPI_CIPO | CircuitPython CIPO (P0.02)
    { MP_ROM_QSTR(MP_QSTR_SPI_MISO), MP_ROM_PTR(&pin_P0_02) },    // MicroMod SPI_MISO | CircuitPython MISO (P0.02)
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI), MP_ROM_PTR(&pin_P0_31) },    // MicroMod SPI_COPI | CircuitPython COPI | LED_DAT (P0.31)
    { MP_ROM_QSTR(MP_QSTR_SPI_MOSI), MP_ROM_PTR(&pin_P0_31) },    // MicroMod SPI_MOSI | CircuitPython MOSI (P0.31)
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK), MP_ROM_PTR(&pin_P0_28) },     // MicroMod SPI_SCK | CircuitPython SCK | LED_CLK (P0.28)
    { MP_ROM_QSTR(MP_QSTR_SPI_CS), MP_ROM_PTR(&pin_P0_20) },      // MicroMod SPI_CS | CircuitPython CS (P0.20)

    // CircuitPython default SPI pins
    { MP_ROM_QSTR(MP_QSTR_CIPO), MP_ROM_PTR(&pin_P0_02) },        // CircuitPython CIPO | MicroMod SPI_CIPO (P0.02)
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_02) },        // CircuitPython MISO | MicroMod SPI_MISO (P0.02)
    { MP_ROM_QSTR(MP_QSTR_COPI), MP_ROM_PTR(&pin_P0_31) },        // CircuitPython COPI | MicroMod SPI_COPI | LED_DAT (P0.31)
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_31) },        // CircuitPython MOSI | MicroMod SPI_MOSI (P0.31)
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_28) },         // CircuitPython SCK | MicroMod SPI_SCK | LED_CLK (P0.28)
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_P0_20) },          // CircuitPython CS | MicroMod SPI_CS (P0.20)

    // MicroMod 2-wire serial LED pins
    { MP_ROM_QSTR(MP_QSTR_LED_DAT), MP_ROM_PTR(&pin_P0_31) },     // MicroMod LED_DAT | SPI_COPI (P0.31)
    { MP_ROM_QSTR(MP_QSTR_LED_CLK), MP_ROM_PTR(&pin_P0_28) },     // MicroMod LED_CLK | SPI_SCK (P0.28)

    // MicroMod SDIO pins
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_P0_19) },    // MicroMod SDIO_SCK | SPI_SCK1 (P0.19)
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_P0_14) },    // MicroMod SDIO_CMD | SPI_COPI1 (P0.14)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_P0_21) },  // MicroMod SDIO_DATA0 | SPI_CIPO1 (P0.21)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_P0_22) },  // MicroMod SDIO_DATA1 (P0.22)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_P0_23) },  // MicroMod SDIO_DATA2 (P0.23)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA3), MP_ROM_PTR(&pin_P1_00) },  // MicroMod SDIO_DATA3 | SPI_CS1 (P1.00)

    // MicroMod SPI1 pins
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO1), MP_ROM_PTR(&pin_P0_21) },   // MicroMod SPI_CIPO1 | SDIO_DATA0 (P0.21)
    { MP_ROM_QSTR(MP_QSTR_SPI_MISO1), MP_ROM_PTR(&pin_P0_21) },   // MicroMod SPI_MISO1 (P0.21)
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI1), MP_ROM_PTR(&pin_P0_14) },   // MicroMod SPI_COPI1 | SDIO_CMD (P0.14)
    { MP_ROM_QSTR(MP_QSTR_SPI_MOSI1), MP_ROM_PTR(&pin_P0_14) },   // MicroMod SPI_MOSI1 (P0.14)
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK1), MP_ROM_PTR(&pin_P0_19) },    // MicroMod SPI_SCK1 | SDIO_SCK (P0.19)
    { MP_ROM_QSTR(MP_QSTR_SPI_CS1), MP_ROM_PTR(&pin_P1_00) },     // MicroMod SPI_CS1 | SDIO_DATA3 (P1.00)

    // MicroMod audio pins (not supported by MDBT50Q-P1M)
    // { MP_ROM_QSTR(MP_QSTR_AUD_MCLK), MP_ROM_PTR() },           // MicroMod AUD_MCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_AUD_OUT), MP_ROM_PTR() },            // MicroMod AUD_OUT | I2S_OUT | PCM_OUT | CAM_MCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_AUD_IN), MP_ROM_PTR() },             // MicroMod AUD_IN | I2S_IN | PCM_IN | CAM_PCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_AUD_LRCLK), MP_ROM_PTR(&pin_P0_26) },// MicroMod AUD_LRCLK | I2S_WS | PCM_SYNC | PDM_DATA (P0.26)
    // { MP_ROM_QSTR(MP_QSTR_AUD_BCLK), MP_ROM_PTR(&pin_P0_25) }, // MicroMod AUD_BCLK | I2S_SCK | PCM_CLK | PDM_CLK (P0.25)

    // MicroMod I2S pins (not supported by MDBT50Q-P1M)
    // { MP_ROM_QSTR(MP_QSTR_I2S_OUT), MP_ROM_PTR() },            // MicroMod I2S_OUT | AUD_OUT | PCM_OUT | CAM_MCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_I2S_IN), MP_ROM_PTR() },             // MicroMod I2S_IN | AUD_IN | PCM_IN | CAM_PCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_I2S_WS), MP_ROM_PTR(&pin_P0_26) },   // MicroMod I2S_WS | AUD_LRCLK | PCM_SYNC | PDM_DATA (P0.26)
    // { MP_ROM_QSTR(MP_QSTR_I2S_SCK), MP_ROM_PTR(&pin_P0_25) },  // MicroMod I2S_SCK | AUD_BCLK | PCM_CLK | PDM_CLK (P0.25)

    // MicroMod PCM pins (not supported by MDBT50Q-P1M)
    // { MP_ROM_QSTR(MP_QSTR_PCM_OUT), MP_ROM_PTR() },            // MicroMod PCM_OUT | AUD_OUT | I2S_OUT | CAM_MCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_PCM_IN), MP_ROM_PTR() },             // MicroMod PCM_IN | AUD_IN | I2S_IN | CAM_PCLK (not connected)
    // { MP_ROM_QSTR(MP_QSTR_PCM_SYNC), MP_ROM_PTR(&pin_P0_26) }, // MicroMod PCM_SYNC | AUD_LRCLK | I2S_WS | PDM_DATA (P0.26)
    // { MP_ROM_QSTR(MP_QSTR_PCM_CLK), MP_ROM_PTR(&pin_P0_25) },  // MicroMod PCM_CLK | AUD_BCLK | I2S_SCK | PDM_CLK (P0.25)

    // MicroMod PDM pins
    { MP_ROM_QSTR(MP_QSTR_PDM_DATA), MP_ROM_PTR(&pin_P0_26) },    // MicroMod PDM_DATA | AUD_LRCLK | I2S_WS | PCM_SYNC (P0.26)
    { MP_ROM_QSTR(MP_QSTR_PDM_CLK), MP_ROM_PTR(&pin_P0_25) },     // MicroMod PDM_CLK | AUD_BCLK | I2S_SCK | PCM_CLK (P0.25)

    // MicroMod SWD pins
    // { MP_ROM_QSTR(MP_QSTR_SWDIO), MP_ROM_PTR() },              // MicroMod SWDIO (MDBT50Q-P1M has a dedicated HW SWDIO pin)
    // { MP_ROM_QSTR(MP_QSTR_SWCLK), MP_ROM_PTR() },              // MicroMod SWDCK (MDBT50Q-P1M has a dedicated HW SWDCLK pin)
    // { MP_ROM_QSTR(MP_QSTR_SWO), MP_ROM_PTR() },                // MicroMod SWO | G11 (not supported)

    // MicroMod ADC pins
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_05) },          // MicroMod A0 (P0.05)
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_04) },          // MicroMod A1 (P0.04)

    // MicroMod PWM pins
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_P0_06) },        // MicroMod PWM0 (P0.06)
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_P0_16) },        // MicroMod PWM1 (P0.16)

    // MicroMod digital pins
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_P0_27) },          // MicroMod D0 (P0.27)
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_P1_08) },          // MicroMod D1 | CAM_TRIG (P1.08)

    // MicroMod general purpose pins
    { MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_P0_29) },          // MicroMod G0 | BUS0 (P0.29)
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_P0_03) },          // MicroMod G1 | BUS1 (P0.03)
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_P1_13) },          // MicroMod G2 | BUS2 (P1.13)
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_P1_12) },          // MicroMod G3 | BUS3 (P1.12)
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_P1_11) },          // MicroMod G4 | BUS4 (P1.11)
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_P0_17) },          // MicroMod G5 | BUS5 (P0.17)
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_P1_06) },          // MicroMod G6 | BUS6 (P1.06)
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_P1_04) },          // MicroMod G7 | BUS7 (P1.04)
    { MP_ROM_QSTR(MP_QSTR_G8), MP_ROM_PTR(&pin_P1_14) },          // MicroMod G8 (P1.14)
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_P0_09) },          // MicroMod G9 | ADC_D- | CAM_HSYNC (P0.09)
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_P0_10) },         // MicroMod G10 | ADC_D+ | CAM_VSYNC (P0.10)
    // { MP_ROM_QSTR(MP_QSTR_G11), MP_ROM_PTR() },                // MicroMod G11 | SWO (not connected)

    // MicroMod 8-bit bus pins
    { MP_ROM_QSTR(MP_QSTR_BUS0), MP_ROM_PTR(&pin_P0_29) },        // MicroMod BUS0 | G0 (P0.29)
    { MP_ROM_QSTR(MP_QSTR_BUS1), MP_ROM_PTR(&pin_P0_03) },        // MicroMod BUS1 | G1 (P0.03)
    { MP_ROM_QSTR(MP_QSTR_BUS2), MP_ROM_PTR(&pin_P1_13) },        // MicroMod BUS2 | G2 (P1.13)
    { MP_ROM_QSTR(MP_QSTR_BUS3), MP_ROM_PTR(&pin_P1_12) },        // MicroMod BUS3 | G3 (P1.12)
    { MP_ROM_QSTR(MP_QSTR_BUS4), MP_ROM_PTR(&pin_P1_11) },        // MicroMod BUS4 | G4 (P1.11)
    { MP_ROM_QSTR(MP_QSTR_BUS5), MP_ROM_PTR(&pin_P0_17) },        // MicroMod BUS5 | G5 (P0.17)
    { MP_ROM_QSTR(MP_QSTR_BUS6), MP_ROM_PTR(&pin_P1_06) },        // MicroMod BUS6 | G6 (P1.06)
    { MP_ROM_QSTR(MP_QSTR_BUS7), MP_ROM_PTR(&pin_P1_04) },        // MicroMod BUS7 | G7 (P1.04)

    // MicroMod differential ADC input pins (not supported by MDBT50Q-P1M)
    // { MP_ROM_QSTR(MP_QSTR_ADC_DM), MP_ROM_PTR(&pin_P0_09) },   // MicroMod ADC_D- | G9 | CAM_HSYNC (P0.09)
    // { MP_ROM_QSTR(MP_QSTR_ADC_DP), MP_ROM_PTR(&pin_P0_10) },   // MicroMod ADC_D+ | G10 | CAM_VSYNC (P0.10)

    // MicroMod camera pins (not supported by MDBT50Q-P1M)
    // { MP_ROM_QSTR(MP_QSTR_CAM_MCLK), MP_ROM_PTR() },           // MicroMod CAM_MCLK | AUD_OUT | I2S_OUT | PCM_OUT (not connected)
    // { MP_ROM_QSTR(MP_QSTR_CAM_PCLK), MP_ROM_PTR() },           // MicroMod CAM_PCLK | AUD_IN | I2S_IN | PCM_IN (not connected)
    // { MP_ROM_QSTR(MP_QSTR_CAM_TRIG), MP_ROM_PTR(&pin_P1_08) }, // MicroMod CAM_TRIG | D1 (P1.08)
    // { MP_ROM_QSTR(MP_QSTR_CAM_HSYNC), MP_ROM_PTR(&pin_P0_09) },// MicroMod CAM_HSYNC | ADC_D- | G9 (P0.09)
    // { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC), MP_ROM_PTR(&pin_P0_10) },// MicroMod CAM_VSYNC | ADC_D+ | G10 (P0.10)

    // CircuitPython board objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },     // CircuitPython I2C
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },     // CircuitPython SPI
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },   // CircuitPython UART
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
