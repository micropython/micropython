// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Adafruit CircuitPlayground Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA17)

#define MICROPY_HW_NEOPIXEL         (&pin_PB23)
#define MICROPY_HW_NEOPIXEL_COUNT   (10)

// Don't allow touch on A0 (PA02), because it's connected to the speaker.
#define PA02_NO_TOUCH       (true)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

// On-board flash
#define SPI_FLASH_MOSI_PIN          &pin_PA20
#define SPI_FLASH_MISO_PIN          &pin_PA16
#define SPI_FLASH_SCK_PIN           &pin_PA21
#define SPI_FLASH_CS_PIN            &pin_PB22

#define SPEAKER_ENABLE_PIN    (&pin_PA30)

#define CALIBRATE_CRYSTALLESS 1

// Explanation of how a user got into safe mode.
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed both buttons at start up.")

// Increase stack size slightly due to CPX library import nesting
#define CIRCUITPY_DEFAULT_STACK_SIZE  (4248) // divisible by 8

#define DEFAULT_I2C_BUS_SCL (&pin_PB03)
#define DEFAULT_I2C_BUS_SDA (&pin_PB02)

#define DEFAULT_SPI_BUS_SCK (&pin_PA05)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA07)
#define DEFAULT_SPI_BUS_MISO (&pin_PA06)

#define DEFAULT_UART_BUS_RX (&pin_PB09)
#define DEFAULT_UART_BUS_TX (&pin_PB08)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
