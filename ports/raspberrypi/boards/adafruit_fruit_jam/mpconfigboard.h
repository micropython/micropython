// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Adafruit Fruit Jam"
#define MICROPY_HW_MCU_NAME "rp2350b"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO32)
#define MICROPY_HW_NEOPIXEL_COUNT (5)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO20)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO30)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO31)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO28)

#define DEFAULT_UART_BUS_RX (&pin_GPIO9)
#define DEFAULT_UART_BUS_TX (&pin_GPIO8)

#define DEFAULT_USB_HOST_DATA_PLUS (&pin_GPIO1)
#define DEFAULT_USB_HOST_DATA_MINUS (&pin_GPIO2)
#define DEFAULT_USB_HOST_5V_POWER (&pin_GPIO11)

#define DEFAULT_DVI_BUS_CLK_DN (&pin_GPIO12)
#define DEFAULT_DVI_BUS_CLK_DP (&pin_GPIO13)
#define DEFAULT_DVI_BUS_RED_DN (&pin_GPIO14)
#define DEFAULT_DVI_BUS_RED_DP (&pin_GPIO15)
#define DEFAULT_DVI_BUS_GREEN_DN (&pin_GPIO16)
#define DEFAULT_DVI_BUS_GREEN_DP (&pin_GPIO17)
#define DEFAULT_DVI_BUS_BLUE_DN (&pin_GPIO18)
#define DEFAULT_DVI_BUS_BLUE_DP (&pin_GPIO19)

#define DEFAULT_SD_SCK (&pin_GPIO34)
#define DEFAULT_SD_MOSI (&pin_GPIO35)
#define DEFAULT_SD_MISO (&pin_GPIO36)
#define DEFAULT_SD_CS (&pin_GPIO39)
#define DEFAULT_SD_CARD_DETECT (&pin_GPIO33)
#define DEFAULT_SD_CARD_INSERTED true

#define CIRCUITPY_PSRAM_CHIP_SELECT (&pin_GPIO47)

// The ESP32-C6 running nina-fw, on board.SPI(). nina-fw raises ESP_IRQ when a
// socket has data, so web workflow polls only then.
#define CIRCUITPY_AIRLIFT_CS (&pin_GPIO46)
#define CIRCUITPY_AIRLIFT_BUSY (&pin_GPIO3)
#define CIRCUITPY_AIRLIFT_RESET (&pin_GPIO22)
#define CIRCUITPY_AIRLIFT_GPIO0 (&pin_GPIO23)

// #define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO44)
// #define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO45)

// #define CIRCUITPY_DEBUG_TINYUSB 0

#define CIRCUITPY_SAVES_PARTITION_SIZE (2 * 1024 * 1024)
