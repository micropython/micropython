// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "8086 USB Interposer"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO7)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO15)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO14)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

// #define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
// #define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX

#define CIRCUITPY_USB_HOST_INSTANCE 1
