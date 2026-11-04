// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Adafruit Metro ESP32-P4"
#define MICROPY_HW_MCU_NAME "ESP32P4"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO23)
#define CIRCUITPY_BOOT_BUTTON (&pin_GPIO35)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO32)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO33)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO15)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO16)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO17)

#define DEFAULT_UART_BUS_RX (&pin_GPIO0)
#define DEFAULT_UART_BUS_TX (&pin_GPIO1)

// USB-C shares GPIO24/25 with the ROM USB Serial/JTAG downloader.
#define CIRCUITPY_USB_DEVICE_INSTANCE 0
#define CIRCUITPY_ESP32P4_SWAP_LSFS (1)

// USB-A is connected to the dedicated high-speed PHY.
#define CIRCUITPY_USB_HOST_INSTANCE 1
