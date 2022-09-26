/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 flom84
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

#define MICROPY_HW_BOARD_NAME "NUCLEO F446RE"
#define MICROPY_HW_MCU_NAME "STM32F446xx"

#define FLASH_SIZE (0x80000u)     // 512K
#define FLASH_PAGE_SIZE (0x4000u) // 16K

#define HSE_VALUE ((uint32_t)8000000u)
#define BOARD_HSE_SOURCE (RCC_HSE_ON)
// The schematic has a 32k crystal that isn't fitted. Uncommented the line below if you add it.
// #define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
// #define LSE_VALUE  ((uint32_t)32000U)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

// USART3 + USB FTDI
// #define CIRCUITPY_CONSOLE_UART_TX (&pin_PC10)
// #define CIRCUITPY_CONSOLE_UART_RX (&pin_PC11)

// USART2 + ST link
// #define CIRCUITPY_CONSOLE_UART_TX (&pin_PA02)
// #define CIRCUITPY_CONSOLE_UART_RX (&pin_PA03)

// Status LEDs
#define MICROPY_HW_LED_STATUS (&pin_PA05)

#define MICROPY_FATFS_EXFAT 0

#define BOARD_NO_VBUS_SENSE (1)
