/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Adafruit Circuit Playground Bluefruit"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS       (&pin_P1_14)

// Board does not have a 32kHz crystal. It does have a 32MHz crystal.
#define BOARD_HAS_32KHZ_XTAL (0)

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 21)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 23)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(1, 00)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 15)
#endif

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_21
#define SPI_FLASH_MISO_PIN &pin_P0_23
#define SPI_FLASH_SCK_PIN &pin_P0_19
#define SPI_FLASH_CS_PIN &pin_P0_15
#endif

// Disables onboard peripherals and neopixels to save power.
#define POWER_SWITCH_PIN            (&pin_P0_06)

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_04)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_05)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_02)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_03)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_29)

#define DEFAULT_UART_BUS_RX         (&pin_P0_30)
#define DEFAULT_UART_BUS_TX         (&pin_P0_14)

#define SPEAKER_ENABLE_PIN          (&pin_P1_04)
