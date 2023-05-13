/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Blues Wireless Contributors.
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

#define MICROPY_HW_BOARD_NAME       "Swan R5"
#define MICROPY_HW_MCU_NAME         "STM32L4R5ZIY6"

// todo - not sure why this isn't the default for this port
#define MICROPY_PY_SYS_PLATFORM     MICROPY_HW_BOARD_NAME


#define STM32L4R5XX
#define BOARD_SWAN_R5


// The flash size is defined in the STM32L4xx HAL (but not for the F4)
// #define FLASH_SIZE                  (0x200000)
// #define FLASH_PAGE_SIZE             (0x1000)

#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
#define BOARD_HAS_HIGH_SPEED_CRYSTAL (0)

// Increase drive strength of 32kHz external crystal, in line with calculations specified in ST AN2867 sections 3.3, 3.4, and STM32L4 datasheet DS12023 Table 58. LSE oscillator characteristics.
// The drive strength RCC_LSEDRIVE_LOW is marginal for the 32kHz crystal oscillator stability, and RCC_LSEDRIVE_MEDIUMLOW meets the calculated drive strength with a small margin for parasitic capacitance.
#define BOARD_LSE_DRIVE_LEVEL RCC_LSEDRIVE_MEDIUMLOW

// Bootloader only
#ifdef UF2_BOOTLOADER_ENABLED
    #define BOARD_VTOR_DEFER (1) // Leave VTOR relocation to bootloader
#endif

#define BOARD_NO_VBUS_SENSE (1)
#define BOARD_NO_USB_OTG_ID_SENSE (1)

#define DEFAULT_I2C_BUS_SCL (&pin_PB06)
#define DEFAULT_I2C_BUS_SDA (&pin_PB07)

#define DEFAULT_SPI_BUS_SS (&pin_PD00)
#define DEFAULT_SPI_BUS_SCK (&pin_PD01)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB15)
#define DEFAULT_SPI_BUS_MISO (&pin_PB14)

#define DEFAULT_UART_BUS_RX (&pin_PA10)
#define DEFAULT_UART_BUS_TX (&pin_PA09)

#define SWAN_R5_DISCHARGE_3V3 (&pin_PE06)
#define SWAN_R5_ENABLE_3V3 (&pin_PE04)
