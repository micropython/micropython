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
    // Note: GPIO25 is connected to both the status LED and edge connector pin
    // G10, which doesn't comply with the requirement above...
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO25) },        // MicroMod LED (GPIO25)

    // MicroMod USB bus input voltage (+5V) pin
    // { MP_ROM_QSTR(MP_QSTR_USB_VIN), MP_ROM_PTR() },            // MicroMod USB_VIN (not connected)

    // MicroMod +3.3V enable pin
    // { MP_ROM_QSTR(MP_QSTR_P3V3_EN), MP_ROM_PTR() },            // MicroMod 3.3V_EN (not connected)

    // MicroMod battery voltage sense pin
    { MP_ROM_QSTR(MP_QSTR_BATT_VIN3), MP_ROM_PTR(&pin_GPIO29) },  // MicroMod BATT_VIN/3 (GPIO29)

    // MicroMod reset pin
    // { MP_ROM_QSTR(MP_QSTR_RESET), MP_ROM_PTR() },              // MicroMod RESET# (RP2040 has a dedicated HW RUN pin)

    // MicroMod boot pin
    // { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR() },               // MicroMod BOOT (RP2040 does not have a dedicated BOOT pin)

    // MicroMod USB device pins
    // USB device is always used internally by CircuitPython, so skip creating
    // the pin objects for it.
    // { MP_ROM_QSTR(MP_QSTR_USB_DM), MP_ROM_PTR() },             // MicroMod USB_D- (RP2040 has a dedicated HW USB_DM pin)
    // { MP_ROM_QSTR(MP_QSTR_USB_DP), MP_ROM_PTR() },             // MicroMod USB_D+ (RP2040 has a dedicated HW USB_DP pin)

    // MicroMod USB host pins
    // { MP_ROM_QSTR(MP_QSTR_USBHOST_DM), MP_ROM_PTR() },         // MicroMod USBHOST_D- (RP2040 has a dedicated HW USB_DM pin)
    // { MP_ROM_QSTR(MP_QSTR_USBHOST_DP), MP_ROM_PTR() },         // MicroMod USBHOST_D+ (RP2040 has a dedicated HW USB_DP pin)

    // MicroMod CAN pins
    // { MP_ROM_QSTR(MP_QSTR_CAN_RX), MP_ROM_PTR() },             // MicroMod CAN_RX (not supported)
    // { MP_ROM_QSTR(MP_QSTR_CAN_TX), MP_ROM_PTR() },             // MicroMod CAN_TX (not supported)

    // Note: MicroMod UART (UART0) is not present in the edge connector pinout
    // because the primary debug serial port is exposed as a virtual serial port
    // over USB.

    // MicroMod UART1 pins
    { MP_ROM_QSTR(MP_QSTR_UART_TX1), MP_ROM_PTR(&pin_GPIO0) },    // MicroMod UART_TX1 | CircuitPython TX (GPIO0)
    { MP_ROM_QSTR(MP_QSTR_UART_RX1), MP_ROM_PTR(&pin_GPIO1) },    // MicroMod UART_RX1 | CircuitPython RX (GPIO1)
    { MP_ROM_QSTR(MP_QSTR_UART_RTS1), MP_ROM_PTR(&pin_GPIO3) },   // MicroMod RTS1 (GPIO3)
    { MP_ROM_QSTR(MP_QSTR_UART_CTS1), MP_ROM_PTR(&pin_GPIO2) },   // MicroMod CTS1 (GPIO2)

    // CircuitPython default UART pins
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO0) },          // CircuitPython TX | MicroMod UART_TX1 (GPIO0)
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO1) },          // CircuitPython RX | MicroMod UART_RX1 (GPIO1)

    // MicroMod UART2 pins
    { MP_ROM_QSTR(MP_QSTR_UART_TX2), MP_ROM_PTR(&pin_GPIO8) },    // MicroMod UART_TX2 (GPIO8)
    { MP_ROM_QSTR(MP_QSTR_UART_RX2), MP_ROM_PTR(&pin_GPIO9) },    // MicroMod UART_RX2 (GPIO9)

    // MicroMod I2C pins
    { MP_ROM_QSTR(MP_QSTR_I2C_SDA), MP_ROM_PTR(&pin_GPIO4) },     // MicroMod I2C_SDA | CircuitPython SDA (GPIO4)
    { MP_ROM_QSTR(MP_QSTR_I2C_SCL), MP_ROM_PTR(&pin_GPIO5) },     // MicroMod I2C_SCL | CircuitPython SCL (GPIO5)

    // CircuitPython default I2C pins
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },         // CircuitPython SDA | MicroMod I2C_SDA (GPIO4)
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO5) },         // CircuitPython SCL | MicroMod I2C_SCL (GPIO5)

    // MicroMod I2C interrupt pin
    { MP_ROM_QSTR(MP_QSTR_I2C_INT), MP_ROM_PTR(&pin_GPIO8) },     // MicroMod I2C_INT (GPIO8)

    // MicroMod I2C1 pins
    // { MP_ROM_QSTR(MP_QSTR_I2C_SDA1), MP_ROM_PTR() },           // MicroMod I2C_SDA1 (not connected)
    // { MP_ROM_QSTR(MP_QSTR_I2C_SCL1), MP_ROM_PTR() },           // MicroMod I2C_SCL1 (not connected)

    // MicroMod SPI pins
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO), MP_ROM_PTR(&pin_GPIO20) },   // MicroMod SPI_CIPO | CircuitPython CIPO (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_SPI_MISO), MP_ROM_PTR(&pin_GPIO20) },   // MicroMod SPI_MISO | CircuitPython MISO (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI), MP_ROM_PTR(&pin_GPIO23) },   // MicroMod SPI_COPI | CircuitPython COPI | LED_DAT (GPIO23)
    { MP_ROM_QSTR(MP_QSTR_SPI_MOSI), MP_ROM_PTR(&pin_GPIO23) },   // MicroMod SPI_MOSI | CircuitPython MOSI (GPIO23)
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK), MP_ROM_PTR(&pin_GPIO22) },    // MicroMod SPI_SCK | CircuitPython SCK | LED_CLK (GPIO22)
    { MP_ROM_QSTR(MP_QSTR_SPI_CS), MP_ROM_PTR(&pin_GPIO21) },     // MicroMod SPI_CS | CircuitPython CS (GPIO21)

    // CircuitPython default SPI pins
    { MP_ROM_QSTR(MP_QSTR_CIPO), MP_ROM_PTR(&pin_GPIO20) },       // CircuitPython CIPO | MicroMod SPI_CIPO (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO20) },       // CircuitPython MISO | MicroMod SPI_MISO (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_COPI), MP_ROM_PTR(&pin_GPIO23) },       // CircuitPython COPI | MicroMod SPI_COPI | LED_DAT (GPIO23)
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO23) },       // CircuitPython MOSI | MicroMod SPI_MOSI (GPIO23)
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO22) },        // CircuitPython SCK | MicroMod SPI_SCK | LED_CLK (GPIO22)
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_GPIO21) },         // CircuitPython CS | MicroMod SPI_CS (GPIO21)

    // MicroMod 2-wire serial LED pins
    { MP_ROM_QSTR(MP_QSTR_LED_DAT), MP_ROM_PTR(&pin_GPIO23) },    // MicroMod LED_DAT | SPI_COPI
    { MP_ROM_QSTR(MP_QSTR_LED_CLK), MP_ROM_PTR(&pin_GPIO22) },    // MicroMod LED_CLK | SPI_SCK

    // MicroMod SDIO pins
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO14) },   // MicroMod SDIO_SCK | SPI_SCK1 (GPIO14)
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO15) },   // MicroMod SDIO_CMD | SPI_COPI1 (GPIO15)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA0), MP_ROM_PTR(&pin_GPIO12) }, // MicroMod SDIO_DATA0 | SPI_CIPO1 (GPIO12)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA1), MP_ROM_PTR(&pin_GPIO11) }, // MicroMod SDIO_DATA1 (GPIO11)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA2), MP_ROM_PTR(&pin_GPIO10) }, // MicroMod SDIO_DATA2 (GPIO10)
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA3), MP_ROM_PTR(&pin_GPIO9) },  // MicroMod SDIO_DATA3 | SPI_CS1 (GPIO9)

    // MicroMod SPI1 pins
    { MP_ROM_QSTR(MP_QSTR_SPI_CIPO1), MP_ROM_PTR(&pin_GPIO12) },  // MicroMod SPI_CIPO1 | SDIO_DATA0 (GPIO12)
    { MP_ROM_QSTR(MP_QSTR_SPI_MISO1), MP_ROM_PTR(&pin_GPIO12) },  // MicroMod SPI_MISO1 (GPIO12)
    { MP_ROM_QSTR(MP_QSTR_SPI_COPI1), MP_ROM_PTR(&pin_GPIO15) },  // MicroMod SPI_COPI1 | SDIO_CMD (GPIO15)
    { MP_ROM_QSTR(MP_QSTR_SPI_MOSI1), MP_ROM_PTR(&pin_GPIO15) },  // MicroMod SPI_MOSI1 (GPIO15)
    { MP_ROM_QSTR(MP_QSTR_SPI_SCK1), MP_ROM_PTR(&pin_GPIO14) },   // MicroMod SPI_SCK1 | SDIO_SCK (GPIO14)
    { MP_ROM_QSTR(MP_QSTR_SPI_CS1), MP_ROM_PTR(&pin_GPIO9) },     // MicroMod SPI_CS1 | SDIO_DATA3 (GPIO9)

    // MicroMod audio pins
    { MP_ROM_QSTR(MP_QSTR_AUD_MCLK), MP_ROM_PTR(&pin_GPIO24) },   // MicroMod AUD_MCLK (GPIO24)
    { MP_ROM_QSTR(MP_QSTR_AUD_OUT), MP_ROM_PTR(&pin_GPIO10) },    // MicroMod AUD_OUT | I2S_OUT | PCM_OUT | CAM_MCLK (GPIO10)
    { MP_ROM_QSTR(MP_QSTR_AUD_IN), MP_ROM_PTR(&pin_GPIO11) },     // MicroMod AUD_IN | I2S_IN | PCM_IN | CAM_PCLK (GPIO11)
    { MP_ROM_QSTR(MP_QSTR_AUD_LRCLK), MP_ROM_PTR(&pin_GPIO2) },   // MicroMod AUD_LRCLK | I2S_WS | PCM_SYNC | PDM_DATA (GPIO2)
    { MP_ROM_QSTR(MP_QSTR_AUD_BCLK), MP_ROM_PTR(&pin_GPIO3) },    // MicroMod AUD_BCLK | I2S_SCK | PCM_CLK | PDM_CLK (GPIO3)

    // MicroMod I2S pins
    { MP_ROM_QSTR(MP_QSTR_I2S_OUT), MP_ROM_PTR(&pin_GPIO10) },    // MicroMod I2S_OUT | AUD_OUT | PCM_OUT | CAM_MCLK (GPIO10)
    { MP_ROM_QSTR(MP_QSTR_I2S_IN), MP_ROM_PTR(&pin_GPIO11) },     // MicroMod I2S_IN | AUD_IN | PCM_IN | CAM_PCLK (GPIO11)
    { MP_ROM_QSTR(MP_QSTR_I2S_WS), MP_ROM_PTR(&pin_GPIO2) },      // MicroMod I2S_WS | AUD_LRCLK | PCM_SYNC | PDM_DATA (GPIO2)
    { MP_ROM_QSTR(MP_QSTR_I2S_SCK), MP_ROM_PTR(&pin_GPIO3) },     // MicroMod I2S_SCK | AUD_BCLK | PCM_CLK | PDM_CLK (GPIO3)

    // MicroMod PCM pins
    { MP_ROM_QSTR(MP_QSTR_PCM_OUT), MP_ROM_PTR(&pin_GPIO10) },    // MicroMod PCM_OUT | AUD_OUT | I2S_OUT | CAM_MCLK (GPIO10)
    { MP_ROM_QSTR(MP_QSTR_PCM_IN), MP_ROM_PTR(&pin_GPIO11) },     // MicroMod PCM_IN | AUD_IN | I2S_IN | CAM_PCLK (GPIO11)
    { MP_ROM_QSTR(MP_QSTR_PCM_SYNC), MP_ROM_PTR(&pin_GPIO2) },    // MicroMod PCM_SYNC | AUD_LRCLK | I2S_WS | PDM_DATA (GPIO2)
    { MP_ROM_QSTR(MP_QSTR_PCM_CLK), MP_ROM_PTR(&pin_GPIO3) },     // MicroMod PCM_CLK | AUD_BCLK | I2S_SCK | PDM_CLK (GPIO3)

    // MicroMod PDM pins
    { MP_ROM_QSTR(MP_QSTR_PDM_DATA), MP_ROM_PTR(&pin_GPIO2) },    // MicroMod PDM_DATA | AUD_LRCLK | I2S_WS | PCM_SYNC (GPIO2)
    { MP_ROM_QSTR(MP_QSTR_PDM_CLK), MP_ROM_PTR(&pin_GPIO3) },     // MicroMod PDM_CLK | AUD_BCLK | I2S_SCK | PCM_CLK (GPIO3)

    // MicroMod SWD pins
    // { MP_ROM_QSTR(MP_QSTR_SWDIO), MP_ROM_PTR() },              // MicroMod SWDIO (RP2040 has a dedicated HW SWDIO pin)
    // { MP_ROM_QSTR(MP_QSTR_SWCLK), MP_ROM_PTR() },              // MicroMod SWDCK (RP2040 has a dedicated HW SWCLK pin)
    // { MP_ROM_QSTR(MP_QSTR_SWO), MP_ROM_PTR() },                // MicroMod SWO | G11 (not supported)

    // MicroMod ADC pins
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },         // MicroMod A0 (GPIO26)
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27) },         // MicroMod A1 (GPIO27)

    // MicroMod PWM pins
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_GPIO13) },       // MicroMod PWM0 (GPIO13)
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_GPIO24) },       // MicroMod PWM1 (GPIO24)

    // MicroMod digital pins
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_GPIO6) },          // MicroMod D0 (GPIO6)
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO7) },          // MicroMod D1 | CAM_TRIG (GPIO7)

    // MicroMod general purpose pins
    { MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_GPIO16) },         // MicroMod G0 | BUS0 (GPIO16)
    { MP_ROM_QSTR(MP_QSTR_G1), MP_ROM_PTR(&pin_GPIO17) },         // MicroMod G1 | BUS1 (GPIO17)
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_GPIO18) },         // MicroMod G2 | BUS2 (GPIO18)
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_GPIO19) },         // MicroMod G3 | BUS3 (GPIO19)
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO20) },         // MicroMod G4 | BUS4 (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_GPIO21) },         // MicroMod G5 | BUS5 (GPIO21)
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_GPIO22) },         // MicroMod G6 | BUS6 (GPIO22)
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_GPIO23) },         // MicroMod G7 | BUS7 (GPIO23)
    // { MP_ROM_QSTR(MP_QSTR_G8), MP_ROM_PTR() },                 // MicroMod G8 (not connected)
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_GPIO28) },         // MicroMod G9 | ADC_D- | CAM_HSYNC (GPIO28)
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_GPIO25) },        // MicroMod G10 | ADC_D+ | CAM_VSYNC (GPIO25)
    // { MP_ROM_QSTR(MP_QSTR_G11), MP_ROM_PTR() },                // MicroMod G11 | SWO (not connected)

    // MicroMod 8-bit bus pins
    { MP_ROM_QSTR(MP_QSTR_BUS0), MP_ROM_PTR(&pin_GPIO16) },       // MicroMod BUS0 | G0 (GPIO16)
    { MP_ROM_QSTR(MP_QSTR_BUS1), MP_ROM_PTR(&pin_GPIO17) },       // MicroMod BUS1 | G1 (GPIO17)
    { MP_ROM_QSTR(MP_QSTR_BUS2), MP_ROM_PTR(&pin_GPIO18) },       // MicroMod BUS2 | G2 (GPIO18)
    { MP_ROM_QSTR(MP_QSTR_BUS3), MP_ROM_PTR(&pin_GPIO19) },       // MicroMod BUS3 | G3 (GPIO19)
    { MP_ROM_QSTR(MP_QSTR_BUS4), MP_ROM_PTR(&pin_GPIO20) },       // MicroMod BUS4 | G4 (GPIO20)
    { MP_ROM_QSTR(MP_QSTR_BUS5), MP_ROM_PTR(&pin_GPIO21) },       // MicroMod BUS5 | G5 (GPIO21)
    { MP_ROM_QSTR(MP_QSTR_BUS6), MP_ROM_PTR(&pin_GPIO22) },       // MicroMod BUS6 | G6 (GPIO22)
    { MP_ROM_QSTR(MP_QSTR_BUS7), MP_ROM_PTR(&pin_GPIO23) },       // MicroMod BUS7 | G7 (GPIO23)

    // MicroMod differential ADC input pins (not supported by RP2040)
    // { MP_ROM_QSTR(MP_QSTR_ADC_DM), MP_ROM_PTR(&pin_GPIO28) },  // MicroMod ADC_D- | G9 | CAM_HSYNC (GPIO28)
    // { MP_ROM_QSTR(MP_QSTR_ADC_DP), MP_ROM_PTR(&pin_GPIO25) },  // MicroMod ADC_D+ | G10 | CAM_VSYNC (GPIO25)

    // MicroMod camera pins
    { MP_ROM_QSTR(MP_QSTR_CAM_MCLK), MP_ROM_PTR(&pin_GPIO10) },   // MicroMod CAM_MCLK | AUD_OUT | I2S_OUT | PCM_OUT (GPIO10)
    { MP_ROM_QSTR(MP_QSTR_CAM_PCLK), MP_ROM_PTR(&pin_GPIO11) },   // MicroMod CAM_PCLK | AUD_IN | I2S_IN | PCM_IN (GPIO11)
    { MP_ROM_QSTR(MP_QSTR_CAM_TRIG), MP_ROM_PTR(&pin_GPIO7) },    // MicroMod CAM_TRIG | D1 (GPIO7)
    { MP_ROM_QSTR(MP_QSTR_CAM_HSYNC), MP_ROM_PTR(&pin_GPIO28) },  // MicroMod CAM_HSYNC | ADC_D- | G9 (GPIO28)
    { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC), MP_ROM_PTR(&pin_GPIO25) },  // MicroMod CAM_VSYNC | ADC_D+ | G10 (GPIO25)

    // CircuitPython board objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },     // CircuitPython I2C
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },     // CircuitPython SPI
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },   // CircuitPython UART
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
