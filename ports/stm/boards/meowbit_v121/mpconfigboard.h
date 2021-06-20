/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "MEOWBIT"
#define MICROPY_HW_MCU_NAME         "STM32F401xE"

#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)

#define AUTORESET_DELAY_MS 500
#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000)

#define HSE_VALUE ((uint32_t)12000000U)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define BOARD_NO_VBUS_SENSE (1)
#define BOARD_VTOR_DEFER (1) // Leave VTOR relocation to bootloader
#define BOARD_USE_INTERNAL_SPI

// On-board flash
#define SPI_FLASH_MOSI_PIN          (&pin_PB15)
#define SPI_FLASH_MISO_PIN          (&pin_PB14)
#define SPI_FLASH_SCK_PIN           (&pin_PB13)
#define SPI_FLASH_CS_PIN            (&pin_PB01)
