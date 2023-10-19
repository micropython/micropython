/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "build-devkit_xg24_brd2601b/pin_functions.h"

// Micropython setup
#define MICROPY_HW_BOARD_NAME "SiLabs xG24 Dev Kit"
#define MICROPY_HW_MCU_NAME EFR32_SERIES_LOWER

#define HSE_VALUE ((uint32_t)8000000)
#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define CIRCUITPY_RGB_STATUS_R (&pin_PD2)
#define CIRCUITPY_RGB_STATUS_G (&pin_PA4)
#define CIRCUITPY_RGB_STATUS_B (&pin_PB0)

// On-board flash
#define SPI_FLASH_MOSI_PIN          (&pin_PC3)
#define SPI_FLASH_MISO_PIN          (&pin_PC2)
#define SPI_FLASH_SCK_PIN           (&pin_PC1)
#define SPI_FLASH_CS_PIN            (&pin_PC0)

#define DEFAULT_I2C_BUS_SDA   (&pin_PC5)
#define DEFAULT_I2C_BUS_SCL   (&pin_PC4)
#define DEFAULT_I2C_PERIPHERAL I2C1

#define DEFAULT_SPI_BUS_SCK   (&pin_PC1)
#define DEFAULT_SPI_BUS_MOSI  (&pin_PC3)
#define DEFAULT_SPI_BUS_MISO  (&pin_PC2)
#define DEFAULT_SPI_BUS_SS    (&pin_PA7)

#define NVM_BYTEARRAY_BUFFER_SIZE 512
#define CIRCUITPY_INTERNAL_NVM_SIZE (512)
#undef MICROPY_USE_INTERNAL_PRINTF
#define MICROPY_USE_INTERNAL_PRINTF (0)
