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

#define FEATHER52840

#define MICROPY_HW_BOARD_NAME       "Adafruit Feather nRF52840 Express"
#define MICROPY_HW_MCU_NAME         "nRF52840"
#define MICROPY_PY_SYS_PLATFORM     "Feather52840"

// #define MICROPY_HW_NEOPIXEL         NRF_GPIO_PIN_MAP(0, 13)

// #define MICROPY_QSPI_DATA0          NRF_GPIO_PIN_MAP(1, 9)
// #define MICROPY_QSPI_DATA1          NRF_GPIO_PIN_MAP(0, 11)
// #define MICROPY_QSPI_DATA2          NRF_GPIO_PIN_MAP(0, 12)
// #define MICROPY_QSPI_DATA3          NRF_GPIO_PIN_MAP(0, 14)
// #define MICROPY_QSPI_SCK            NRF_GPIO_PIN_MAP(0, 8)
// #define MICROPY_QSPI_CS             NRF_GPIO_PIN_MAP(1, 8)

// #define MICROPY_HW_UART_RX          NRF_GPIO_PIN_MAP(0, 8)
// #define MICROPY_HW_UART_TX          NRF_GPIO_PIN_MAP(0, 6)
// #define MICROPY_HW_UART_HWFC        (0)

#define CIRCUITPY_AUTORELOAD_DELAY_MS 500

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define PORT_HEAP_SIZE              (128 * 1024)
// TODO #define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

// TODO #include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 1
#define EXTERNAL_FLASH_DEVICES GD25Q16C

#define EXTERNAL_FLASH_QSPI_DUAL

// TODO include "external_flash/external_flash.h"

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         NRF_GPIO_PIN_MAP(1, 11)
#define DEFAULT_I2C_BUS_SDA         NRF_GPIO_PIN_MAP(1, 12)

#define DEFAULT_SPI_BUS_SCK         NRF_GPIO_PIN_MAP(0, 20)
#define DEFAULT_SPI_BUS_MOSI        NRF_GPIO_PIN_MAP(0, 23)
#define DEFAULT_SPI_BUS_MISO        NRF_GPIO_PIN_MAP(0, 22)

#define DEFAULT_UART_BUS_RX         NRF_GPIO_PIN_MAP(1, 0)
#define DEFAULT_UART_BUS_TX         NRF_GPIO_PIN_MAP(0, 24)
