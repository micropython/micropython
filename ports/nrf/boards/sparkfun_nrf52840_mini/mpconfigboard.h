/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#define MICROPY_HW_BOARD_NAME       "SparkFun Pro nRF52840 Mini"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_11)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_08)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_30)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_03)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_31)

#define DEFAULT_UART_BUS_RX         (&pin_P0_15)
#define DEFAULT_UART_BUS_TX         (&pin_P0_17)

/* Note: Flash chip is not provided on SparkFun nRF52840 Mini.
 * Leaving this as a reminder for future/similar versions of the board. */
// Flash operation mode is determined by MICROPY_QSPI_DATAn pin configuration.
// A pin config is valid if it is defined and its value is not 0xFF.
// Quad   mode: If all DATA0 --> DATA3 are valid
// Dual   mode: If DATA0 and DATA1 are valid while either DATA2 and/or DATA3 are invalid
// Single mode: If only DATA0 is valid
/*#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 21)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 23)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
#define MICROPY_QSPI_CS NRF_GPIO_PIN_MAP(0, 17)
#endif

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_20
#define SPI_FLASH_MISO_PIN &pin_P0_21
#define SPI_FLASH_SCK_PIN &pin_P0_19
#define SPI_FLASH_CS_PIN &pin_P0_17
#endif*/
